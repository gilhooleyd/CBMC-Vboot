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

int nondet_int();

VbKeyBlockHeader block_real;
VbKeyBlockHeader *block;

VbPublicKey key_real;
VbPublicKey *key;

RSAPublicKey rsaKey_real;
RSAPublicKey *rsaKey;

uint32_t malloc_addr = 0x100;
uint32_t malloc(int size) {
    uint32_t old = malloc_addr;
    malloc_addr += size;
    return old;
}

int digestResult = 0;
uint64_t size = 0;

int SafeMemcmp(const void *s1, const void *s2, size_t n) {
    return 0;
}

 /*
  * Takes the SAHA1 of buff, length
  */
 uint8_t* DigestBuf(const uint8_t* buf, uint64_t len, int sig_algorithm) {
     uint8_t *result = malloc(1);
     *result = 0;
     // our "result" is only correct if we are hashing the 
     // entire block
     if (buf == (uint8_t *)block) {
         if (len == size) {
             digestResult = 1;
             *result = 1;
         }
     }
     return result;
 }
 
 /*
  * Decrypts sig with key, compares result against hash
  */
 int RSAVerify(const RSAPublicKey *key,
               const uint8_t *sig,
               const uint32_t sig_len,
               const uint8_t sig_type,
               const uint8_t *hash) {
     return *hash;
 }

int main() {
    int ret; 
//    block = &block_real;
    block = malloc(sizeof(*block));
    block->magic[0] = 'C';
    block->magic[1] = 'H';
    block->magic[2] = 'R';
    block->magic[3] = 'O';
    block->magic[4] = 'M';
    block->magic[5] = 'E';
    block->magic[6] = 'O';
    block->magic[7] = 'S';
    block->header_version_major = KEY_BLOCK_HEADER_VERSION_MAJOR;

    size = nondet_int();
    uint64_t og_size = nondet_int();
    block->key_block_size = og_size;
    block->key_block_signature.sig_offset = nondet_int();
    block->key_block_signature.sig_size   = nondet_int();
    block->key_block_signature.data_size  = nondet_int();

    block->key_block_checksum.sig_offset = nondet_int();
    block->key_block_checksum.sig_size   = nondet_int();
    block->key_block_checksum.data_size  = nondet_int();

    block->key_block_flags = nondet_int();

    block->data_key.key_offset = nondet_int();
    block->data_key.key_size = nondet_int();

    // Algorithm 0 should be SHA1, RSA 1024 (sha_utility.c)
    block->data_key.algorithm = 0;
    block->data_key.key_version = nondet_int();

    key = malloc(sizeof(*key));
    key->key_offset = nondet_int();
    key->key_size = 1024; 
    key->algorithm = 0; 
    key->key_version = nondet_int();

    assert((og_size == block->key_block_size));
    ret = KeyBlockVerify(block, size, key, 1);

    if (ret == 0) {
        assert((size >= block->key_block_size));
        assert((size >= block->data_key.key_offset + block->data_key.key_size));
        assert((size >= block->key_block_signature.sig_offset + block->key_block_signature.sig_size));
        assert(digestResult);
    }
}
