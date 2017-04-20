#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gbb_header.h"
#include "host_common.h"
#include "load_firmware_fw.h"
#include "test_common.h"
#include "vboot_common.h"
#include "vboot_nvstorage.h"
#include "vboot_struct.h"

int nondet_int ();
int preambleFailed;
int preambleHashFailed;
int fwHashFailed;

/* Mock data */
static VbCommonParams cparams;
static VbSelectFirmwareParams fparams;
#ifdef MALLOC
static VbKeyBlockHeader *vblock;
static VbFirmwarePreambleHeader *mpreamble;
#else
static VbKeyBlockHeader vblock[2];
static VbFirmwarePreambleHeader mpreamble[2];
#endif
static VbNvContext vnc;
static uint8_t shared_data[VB_SHARED_DATA_MIN_SIZE];
static uint8_t gbb_data[sizeof(GoogleBinaryBlockHeader) + 2048];
// TODO: malloc these?
static VbSharedDataHeader* shared = (VbSharedDataHeader*)shared_data;
static GoogleBinaryBlockHeader* gbb = (GoogleBinaryBlockHeader*)gbb_data;
static RSAPublicKey data_key;
static uint32_t digest_size;
static uint8_t* digest_returned;
static uint8_t* digest_expect_ptr;
static int hash_fw_index;

static uint32_t gbb_flags;

#define TEST_KEY_DATA	\
    "Test contents for the root key this should be 64 chars long."

void *Memset(void *d, const uint8_t c, uint64_t n) {
    return d;
}

uint32_t *preamble_ptr = 0;
uint32_t *block_ptr = 0;

/* Reset mock data (for use before each test) */
static void ResetMocks(void) {
  VbPublicKey *root_key;
  uint8_t *root_key_data;
  int i;

  Memset(&cparams, 0, sizeof(cparams));
  cparams.shared_data_blob = shared_data;
  cparams.gbb_data = gbb_data;
  cparams.gbb_size = sizeof(gbb_data);
  cparams.gbb = gbb;

  Memset(&fparams, 0, sizeof(fparams));
  fparams.verification_block_A = vblock;
  fparams.verification_size_A = sizeof(VbKeyBlockHeader);
  fparams.verification_block_B = vblock + 1;
  fparams.verification_size_B = sizeof(VbKeyBlockHeader);

  Memset(vblock, 0, sizeof(vblock));
  Memset(mpreamble, 0, sizeof(mpreamble));
  for (i = 0; i < 2; i++) {
    /* Default verification blocks to working in all modes */
#if defined(NONDET_VARS)
      vblock[i].key_block_flags = nondet_int();
      vblock[i].data_key.key_version = nondet_int();

      mpreamble[i].firmware_version = nondet_int();
      mpreamble[i].flags = nondet_int();

      // header_version_major is the return value for KeyBlockVerify
      vblock[i].header_version_major = nondet_int();
      // header_version_major is the return value for VerifyFirmwarePreamble
      preambleFailed = nondet_int();
      mpreamble[i].header_version_major = nondet_int();
      // sig_offset is the return value for HashFirmwareBody
      mpreamble[i].body_signature.sig_offset = nondet_int();
      fwHashFailed = nondet_int();
      // sig_size is the return value for VerifyDigest
      preambleHashFailed = nondet_int();
      mpreamble[i].body_signature.sig_size = nondet_int();
#else 
      vblock[i].key_block_flags = 0x0F;
      vblock[i].data_key.key_version = 2;

      mpreamble[i].firmware_version = 4;
#endif

    /* Fix up offsets to preambles */
     vblock[i].key_block_size =
         (uint8_t*)(mpreamble + i) - (uint8_t*)(vblock + i);
    
    mpreamble[i].header_version_minor = 1;  /* Supports preamble flags */

    /* Point kernel subkey to some data following the key header */
    PublicKeyInit(&mpreamble[i].kernel_subkey,
                  (uint8_t*)&mpreamble[i].body_signature, 20);
    mpreamble[i].kernel_subkey.algorithm = 7 + i;
    // TODO: Fix this offset
    mpreamble[i].body_signature.data_size = 20000 + 1000 * i;
  }

  Memset(&vnc, 0, sizeof(vnc));
  VbNvSetup(&vnc);

  Memset(&shared_data, 0, sizeof(shared_data));
  VbSharedDataInit(shared, sizeof(shared_data));

  Memset(&gbb_data, 0, sizeof(gbb_data));

#if defined(NONDET_VARS)
  shared->fw_version_tpm = nondet_int();

  gbb_flags = nondet_int();
  // gbb flags shouldn't be set with disable rollback 
  gbb_flags &= ~GBB_FLAG_DISABLE_FW_ROLLBACK_CHECK;
  gbb->flags = gbb_flags;
#else
  shared->fw_version_tpm = 0x00020004;
  gbb->flags = 0;
#endif

  gbb->rootkey_offset = sizeof(GoogleBinaryBlockHeader);
  root_key = (VbPublicKey *)(gbb_data + gbb->rootkey_offset);
  root_key_data = (uint8_t *)(root_key + 1);
  strcpy((char *)root_key_data, TEST_KEY_DATA);
  PublicKeyInit(root_key, (uint8_t *)root_key_data, sizeof(TEST_KEY_DATA));

  gbb->major_version = GBB_MAJOR_VER;
  gbb->minor_version = GBB_MINOR_VER;
  

  Memset(&data_key, 0, sizeof(data_key));

  digest_size = 1234;
  digest_returned = NULL;
  digest_expect_ptr = NULL;
  hash_fw_index = -1;
}

/****************************************************************************/
/* Mocked verification functions */

static VbError_t VbGbbReadKey(VbCommonParams *cparams, uint32_t offset,
			      VbPublicKey **keyp)
{
    *keyp = (VbPublicKey *) (cparams->gbb_data + offset);
	return VBERROR_SUCCESS;
}

VbError_t VbGbbReadRootKey(VbCommonParams *cparams, VbPublicKey **keyp)
{
	return VbGbbReadKey(cparams, cparams->gbb->rootkey_offset, keyp);
}

VbError_t VbGbbReadRecoveryKey(VbCommonParams *cparams, VbPublicKey **keyp)
{
	return VbGbbReadKey(cparams, cparams->gbb->recovery_key_offset, keyp);
}

int KeyBlockVerify(const VbKeyBlockHeader* block, uint64_t size,
                   const VbPublicKey *key, int hash_only) {

  TEST_EQ(hash_only, 0, "  Don't verify firmware with hash");

  /*
   * We cannot check the address of key, since it will be allocated. We
   * check the contents instead.
   */
  TEST_STR_EQ((char *)GetPublicKeyDataC(key), TEST_KEY_DATA,
              "  Verify with root key");
  TEST_NEQ(block==vblock || block==vblock+1, 0, "  Verify a valid key block");
  
  /* Mock uses header_version_major to hold return value */
  return block->header_version_major;
}

int VerifyFirmwarePreamble(const VbFirmwarePreambleHeader* preamble,
                           uint64_t size, const RSAPublicKey* key) {
    uint32_t preamble1 = preamble;
    uint32_t preamble2 = mpreamble;
    
  TEST_PTR_EQ(key, &data_key, "  Verify preamble data key");
  TEST_NEQ(preamble1==preamble2 || preamble1==preamble2+1, 0,
           "  Verify a valid preamble");

  /* Mock uses header_version_major to hold return value */
  return preambleFailed;
}

RSAPublicKey* PublicKeyToRSA(const VbPublicKey* key) {
  /* Mock uses algorithm!0 to mean invalid key */
  if (key->algorithm)
    return NULL;
  /* Mock uses data key len to hold number of alloc'd keys */
  data_key.len++;
  return &data_key;
}

void RSAPublicKeyFree(RSAPublicKey* key) {
  TEST_PTR_EQ(key, &data_key, "  RSA data key");
  data_key.len--;
}

void DigestInit(DigestContext* ctx, int sig_algorithm) {
  digest_size = 0;
}

void DigestUpdate(DigestContext* ctx, const uint8_t* data, uint32_t len) {
  TEST_PTR_EQ(data, digest_expect_ptr, "  Digesting expected data");
  digest_size += len;
}

uint8_t* DigestFinal(DigestContext* ctx) {
  digest_returned = (uint8_t*)VbExMalloc(4);
  return digest_returned;
}

VbError_t VbExHashFirmwareBody(VbCommonParams* cparams,
                               uint32_t firmware_index) {
  if (VB_SELECT_FIRMWARE_A == firmware_index)
    hash_fw_index = 0;
  else if (VB_SELECT_FIRMWARE_B == firmware_index)
    hash_fw_index = 1;
  else
    return VBERROR_INVALID_PARAMETER;

  digest_expect_ptr = (uint8_t*)(vblock + hash_fw_index) + 5;
  VbUpdateFirmwareBodyHash(
      cparams, digest_expect_ptr,
      mpreamble[hash_fw_index].body_signature.data_size - 1024);
  VbUpdateFirmwareBodyHash(cparams, digest_expect_ptr, 1024);

  // TODO: Figure out what is going on here
  /* If signature offset is 42, hash the wrong amount and return success */
//  if (42 == mpreamble[hash_fw_index].body_signature.sig_offset) {
//    VbUpdateFirmwareBodyHash(cparams, digest_expect_ptr, 4);
//    return VBERROR_SUCCESS;
//  }

  /* Otherwise, mocked function uses body signature offset as returned value */
  return fwHashFailed;
}

int VerifyDigest(const uint8_t* digest, const VbSignature *sig,
                 const RSAPublicKey* key) {
  TEST_PTR_EQ(digest, digest_returned, "Verifying expected digest");
  TEST_PTR_EQ(key, &data_key, "Verifying using data key");
  TEST_PTR_EQ(sig, &mpreamble[hash_fw_index].body_signature, "Verifying sig");

  /* Mocked function uses sig size as return value for verifying digest */
  return preambleHashFailed;
}

/****************************************************************************/

  /* Require GBB */
  /* Key block flags must match */
  /* Test key block verification with A and key version rollback with B */
  /* Test invalid key version with A and bad data key with B */
  /* Test invalid key version with GBB bypass-rollback flag */
  /* Test invalid preamble with A */
  /* Test invalid firmware versions */
  /* Test invalid firmware versions with GBB bypass-rollback flag */
  /* Test RO normal with A */
  /* If RO normal is supported, don't need to verify the firmware body */
  /* If both A and B are valid and same version as TPM, A is selected
   * and B isn't attempted. */
  /* But if try B flag is set, B is selected and A not attempted */
  /* There's a optimistic boot mode that doesn't consume tries.
   * The behaviour should differ only in that the try count doesn't change. */
  /* Optimistic boot case 1: count == 0: Go for A */
  /* Optimistic boot case 2: count > 0: count unchanged, use B */
  /* If both A and B are valid and grater version than TPM, A is
   * selected and B preamble (but not body) is verified. */
  /* Verify firmware data */
  /* Test error getting firmware body */
  /* Test digesting the wrong amount */
  /* Test bad signature */
  /* Test unable to store kernel data key */

int ifThen(unsigned char a, unsigned char b) {
    return !a || b;
}

void checkRollback(uint32_t tpm_prev_version) {
    // is the index of the fw that LoadFirmware chose to load. 
    int i = shared->firmware_index;

    // calculates the total version from the key and the firmware image
    uint32_t combined_version = (uint32_t) (
            (vblock[i].data_key.key_version << 16) |
            (mpreamble[i].firmware_version & 0xFFFF));

    // The new version must always be greater than or equal to what we last saw
    // (remember that LoadFirmware has already passed at this point)
    __CPROVER_assert(combined_version >= tpm_prev_version,
            "Firmware Rollback Happened");
}

void checkIncorrectHash() {
    int i = shared->firmware_index;
    __CPROVER_assert(vblock[i].header_version_major == 0, "Key Block failed");
    __CPROVER_assert(preambleFailed == 0, "FW Preamble Failed");
    __CPROVER_assert(fwHashFailed== 0, "FW Hashing Failed");
    __CPROVER_assert(preambleHashFailed == 0, "Verified Hashing Failed");
}

uint32_t malloc_addr = 0x100;
uint32_t malloc(int size) {
    uint32_t old = malloc_addr;
    malloc_addr += size;
    return old;
}

int main(void) {
    int ret;
    uint32_t tpm_fw_version;

#ifdef MALLOC
    vblock = malloc(sizeof(VbKeyBlockHeader)*2);
    mpreamble = malloc(sizeof(VbFirmwarePreambleHeader)*2);
#else
#endif
//    __CPROVER_assume (mpreamble == 0xffff0000);
//    __CPROVER_assume (vblock    == 0xfffff000);

    ResetMocks();
    tpm_fw_version = shared->fw_version_tpm;

    ret = LoadFirmware(&cparams, &fparams, &vnc);

    if (ret == 0) {
        checkRollback(tpm_fw_version);
        checkIncorrectHash();
    }

    return 0;
}

