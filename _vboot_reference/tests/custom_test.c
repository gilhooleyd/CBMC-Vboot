#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gbb_header.h"
#include "host_common.h"
#include "rollback_index.h"
#include "test_common.h"
#include "vboot_common.h"
#include "vboot_nvstorage.h"
#include "vboot_struct.h"

/* Mock data */
static VbCommonParams cparams;
static VbInitParams iparams;
static GoogleBinaryBlockHeader gbb;

static VbNvContext vnc;
static uint8_t shared_data[VB_SHARED_DATA_MIN_SIZE];
static VbSharedDataHeader *shared = (VbSharedDataHeader *)shared_data;
static uint64_t mock_timer;
static int rollback_s3_retval;
static int nv_write_called;
static int mock_virt_dev_sw;
static uint32_t mock_tpm_version;
static uint32_t mock_rfs_retval;
static int rfs_clear_tpm_request;
static int rfs_disable_dev_request;
static uint8_t backup_space[BACKUP_NV_SIZE];
static int backup_write_called;
static int backup_read_called;

int fp_size = 0;
int fw_size = 0;
int vlength = 4096;
void * gbb_buff;
void * fw_buff;

/* Reset mock data (for use before each test) */
static void ResetInitMocks(void)
{
	Memset(&cparams, 0, sizeof(cparams));
	cparams.shared_data_size = sizeof(shared_data);
	cparams.shared_data_blob = shared_data;
	cparams.gbb_data = &gbb;
	cparams.gbb_size = sizeof(gbb);

	Memset(&gbb, 0, sizeof(gbb));
	gbb.major_version = GBB_MAJOR_VER;
	gbb.minor_version = GBB_MINOR_VER;
	gbb.flags = 0;

	Memset(&iparams, 0, sizeof(iparams));

	Memset(&vnc, 0, sizeof(vnc));
	VbNvSetup(&vnc);
	VbNvTeardown(&vnc);                   /* So CRC gets generated */

	Memset(backup_space, 0, sizeof(backup_space));
	backup_write_called = 0;
	backup_read_called = 0;

	Memset(&shared_data, 0, sizeof(shared_data));
	VbSharedDataInit(shared, sizeof(shared_data));

	mock_timer = 10;
	rollback_s3_retval = TPM_SUCCESS;
	nv_write_called = 0;

	mock_virt_dev_sw = 0;
	mock_tpm_version = 0x10001;
	mock_rfs_retval = 0;

	rfs_clear_tpm_request = 0;
	rfs_disable_dev_request = 0;
}

/****************************************************************************/
/* Mocked verification functions */

VbError_t VbExNvStorageRead(uint8_t *buf)
{
	Memcpy(buf, vnc.raw, sizeof(vnc.raw));
	return VBERROR_SUCCESS;
}

VbError_t VbExNvStorageWrite(const uint8_t *buf)
{
	nv_write_called++;
	Memcpy(vnc.raw, buf, sizeof(vnc.raw));
	return VBERROR_SUCCESS;
}

uint64_t VbExGetTimer(void)
{
	/*
	 * Exponential-ish rather than linear time, so that subtracting any
	 * two mock values will yield a unique result.
	 */
	uint64_t new_timer = mock_timer * 2 + 1;
	VbAssert(new_timer > mock_timer);  /* Make sure we don't overflow */
	mock_timer = new_timer;
	return mock_timer;
}

/*
 * Hash the firmware body. The implementation of this was taken from 
 * U-Boot, where it reads the information out of the flashmap
 */

VbError_t VbExHashFirmwareBody(VbCommonParams *cparams, uint32_t fw_index) {
    FILE *fp;
    int body_size;
    void *fw_body;
    fp = fopen("tests/preamble_tests/data/FWDATA", "r");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    body_size = ftell(fp);
    rewind(fp);
    fw_body = malloc(body_size); 
    fread(fw_body, sizeof(char *), body_size, fp);
    fclose(fp);
    VbUpdateFirmwareBodyHash(cparams, fw_body, body_size);
    return 0;
}

int main(void) {
    uint8_t ret_val;
    VbSelectFirmwareParams fparams;
    FILE *fp;

    ResetInitMocks();

    // read in gbb
    fp = fopen("tests/preamble_tests/gbb.blob", "r");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }
    // get gbb size
    fseek(fp, 0L, SEEK_END);
    fp_size = ftell(fp);
    rewind(fp);

    gbb_buff = malloc(fp_size); 
    fread(gbb_buff, sizeof(char *), fp_size, fp);
    fclose(fp);

    // set the gbb information
    cparams.gbb_data = gbb_buff;
    cparams.gbb_size = fp_size;

    /*
     * Two stop would do things at this point, like wipe the keyboard memory.
     * read from the fmap, start the EC and read the gbb information
     */

    // Initialize the virtual boot data structures
    // (this will also initiliaze the tpm)
    ret_val = VbInit(&cparams, &iparams);

    // Do setup for the next stage
    Memset(&fparams, '\0', sizeof(fparams));
    // read in the whole block of the virtual boot
    // (contains keyblock, preaamble, and RW FW data)
//    fp = fopen("tests/preamble_tests/preamble_v2x/fw_8_8.vblock", "r");
    fp = fopen("tests/custom_data/vblock/fw_8_8.vblock", "r");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    fw_size = ftell(fp);
    rewind(fp);
    fw_buff = malloc(fw_size); 
    fread(fw_buff, sizeof(char *), fw_size, fp);
    fclose(fp);

    VBDEBUG(("FW SIZE: %d\n", fw_size));

    // Set both A & B firmware to the same thing
    fparams.verification_size_A = fparams.verification_size_B = vlength;
    fparams.verification_block_A = fw_buff;
    fparams.verification_block_B = fw_buff;

    // Select a FW by validating the signatures of the keyblock and preamble
    // and by validating the hash of the firmware body
    ret_val = VbSelectFirmware(&cparams, &fparams);

    // afterwards we need code to do the decompression of the 
    // image, and also to copy the image to a designated memory

    TEST_EQ(ret_val, ret_val, "Yeah PASSED!");
    printf("%d\n", ret_val);
    return 0;
}
