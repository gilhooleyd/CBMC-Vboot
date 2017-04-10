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


int main() {
    int ret; 
//    void * ptr = (void *) &block_real;
//    ptr += (void *) &key_real;
//    ptr -= (void *) &key_real;
//
//    assert(ptr == block_real);

    block = &block_real;
    block->magic[0] = 'C';
    block->magic[1] = 'H';
    block->magic[2] = 'R';
    block->magic[3] = 'O';
    block->magic[4] = 'M';
    block->magic[5] = 'E';
    block->magic[6] = 'O';
    block->magic[7] = 'S';
    block->header_version_major = KEY_BLOCK_HEADER_VERSION_MAJOR;

    uint64_t size = nondet_int();
    block->key_block_size = nondet_int();
    block->key_block_signature.sig_offset = nondet_int();
    block->key_block_signature.sig_size   = nondet_int();
    block->key_block_signature.data_size  = nondet_int();

    block->key_block_checksum.sig_offset = nondet_int();
    block->key_block_checksum.sig_size   = nondet_int();
    block->key_block_checksum.data_size  = nondet_int();

    block->key_block_flags = nondet_int();

    block->data_key.key_offset = nondet_int();
    block->data_key.key_size = nondet_int();
    // TODO: get a good algorithm
    block->data_key.algorithm = nondet_int();
    block->data_key.key_version = nondet_int();

    ret = KeyBlockVerify(block, size, &key, 0);

    if (ret == 0) {
        assert((size >= block->key_block_size));
        assert((size > block->data_key.key_offset + block->data_key.key_size));
        assert((size > block->key_block_signature.sig_offset + block->key_block_signature.sig_size));
    }
}
