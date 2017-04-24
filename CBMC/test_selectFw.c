/* Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Tests for vboot_api_firmware
 */

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

/* Flags for mock_*_got_flags variables */
#define MOCK_DEV_FLAG 0x01     /* Developer parameter non-zero */
#define MOCK_REC_FLAG 0x02     /* Recovery parameter non-zero */

/* Mock data */
static VbCommonParams cparams;
static VbSelectFirmwareParams fparams;
static GoogleBinaryBlockHeader gbb;
static VbNvContext vnc;
static uint8_t shared_data[VB_SHARED_DATA_MIN_SIZE];
static VbSharedDataHeader* shared = (VbSharedDataHeader*)shared_data;
static uint64_t mock_timer;
static int nv_write_called;
/* Mock TPM versions */
static uint32_t mock_tpm_version;
static uint32_t mock_lf_tpm_version;  /* TPM version set by LoadFirmware() */
/* Variables for tracking params passed to mock functions */
static uint32_t mock_stbms_got_flags;
static uint64_t mock_stbms_got_fw_flags;
static int mock_rfl_called;
/* Mock return values, so we can simulate errors */
static VbError_t mock_rfw_retval;
static VbError_t mock_rfl_retval;
static VbError_t mock_lf_retval;
static VbError_t mock_stbms_retval;

int nondet_int();

void *Memset(void *d, const uint8_t c, uint64_t n) {
    return d;
}

/* Reset mock data (for use before each test) */
static void ResetMocks(void) {
  Memset(&cparams, 0, sizeof(cparams));
  cparams.shared_data_size = sizeof(shared_data);
  cparams.shared_data_blob = shared_data;

  Memset(&fparams, 0, sizeof(fparams));

  Memset(&gbb, 0, sizeof(gbb));
  cparams.gbb_data = &gbb;
  cparams.gbb_size = sizeof(gbb);
  cparams.gbb = &gbb;

  Memset(&vnc, 0, sizeof(vnc));
  VbNvSetup(&vnc);
  VbNvTeardown(&vnc);  /* So CRC gets generated */

  Memset(&shared_data, 0, sizeof(shared_data));
  VbSharedDataInit(shared, sizeof(shared_data));
  shared->fw_keyblock_flags = 0xABCDE0;

  mock_timer = 10;
  nv_write_called = mock_rfl_called = 0;

  mock_stbms_got_flags = 0;
  mock_stbms_got_fw_flags = 0;

  mock_tpm_version = mock_lf_tpm_version = 0x20004;
  shared->fw_version_tpm_start = mock_tpm_version;
  mock_rfw_retval = mock_rfl_retval = 0;
  mock_lf_retval = mock_stbms_retval = 0;
}

/****************************************************************************/
/* Mocked verification functions */

VbError_t VbExNvStorageRead(uint8_t* buf) {
  Memcpy(buf, vnc.raw, sizeof(vnc.raw));
  return VBERROR_SUCCESS;
}

VbError_t VbExNvStorageWrite(const uint8_t* buf) {
  nv_write_called = 1;
  Memcpy(vnc.raw, buf, sizeof(vnc.raw));
  return VBERROR_SUCCESS;
}

uint64_t VbExGetTimer(void) {
  /* Exponential-ish rather than linear time, so that subtracting any
   * two mock values will yield a unique result. */
  uint64_t new_timer = mock_timer * 2 + 1;
  VbAssert(new_timer > mock_timer);  /* Make sure we don't overflow */
  mock_timer = new_timer;
  return mock_timer;
}

uint32_t RollbackFirmwareWrite(uint32_t version) {
  mock_tpm_version = version;
  return mock_rfw_retval;
}

uint32_t RollbackFirmwareLock(void) {
  mock_rfl_called = 1;
  return mock_rfl_retval;
}

uint32_t SetTPMBootModeState(int developer_mode, int recovery_mode,
			     uint64_t fw_keyblock_flags,
			     GoogleBinaryBlockHeader *gbb) {
  if (recovery_mode)
    mock_stbms_got_flags |= MOCK_REC_FLAG;
  if (developer_mode)
    mock_stbms_got_flags |= MOCK_DEV_FLAG;

  mock_stbms_got_fw_flags = fw_keyblock_flags;

  return mock_stbms_retval;
}

int LoadFirmware(VbCommonParams* cparams, VbSelectFirmwareParams* fparams,
                 VbNvContext* vnc) {
  shared->fw_version_tpm = mock_lf_tpm_version;
  return mock_lf_retval;
}


int main(int argc, char* argv[]) {
    int retval;
    int rec_reason;
    uint32_t shared_flags = sizeof(cparams);
    ResetMocks();
    
    mock_lf_retval = nondet_int();
    // set TPM bootmode state return value
    mock_stbms_retval = nondet_int();
    // RollBackFirmwareLock error
    mock_rfl_retval = nondet_int();

    // shared recovery reason
    rec_reason = nondet_int();
    shared->recovery_reason = rec_reason;

    // shared flags (including developer mode
    shared_flags = nondet_int();
    shared->flags = shared_flags;
    
    // version stored in a 'fake' tpm
    mock_tpm_version = nondet_int();
    shared->fw_version_tpm_start = mock_tpm_version;

    // version found by 'fake' loadfirmware
    mock_lf_tpm_version = nondet_int();

    retval = VbSelectFirmware(&cparams, &fparams);

    if (retval == 0) {

        // if we were not in recovery mode
        if (shared->recovery_reason == 0) {
#ifdef TPM_LOCK
            // tpm boot mode couldn't have errored
            assert(mock_stbms_retval == 0);
            // we must have called lock fw version
            assert(mock_rfl_called != 0);
            // if the tpm was updated in shared
            if (shared->fw_version_tpm > shared->fw_version_tpm_start) {
                // then the tpm mock hardware was updated
                assert(mock_tpm_version == mock_lf_tpm_version);
                // and the tpm couldn't have had an error
                assert(mock_rfl_retval == 0);
            }
#endif
#ifdef LOAD_FIRMWARE
            // loadfirmware couldn't have errored
            assert(mock_lf_retval  == 0);
#endif

      }
      else {
#ifdef TPM_LOCK
            assert(mock_rfl_called == 0);
#endif
        }
    }
    return 0;
}
