/* Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Display functions used in kernel selection.
 */

#ifndef VBOOT_REFERENCE_VBOOT_AUDIO_H_
#define VBOOT_REFERENCE_VBOOT_AUDIO_H_

#include "vboot_api.h"

typedef struct VbAudioContext VbAudioContext;

/* Initialization function. Returns context for processing dev-mode delay */
VbAudioContext* VbAudioOpen(VbCommonParams* cparams);

/* Caller should loop without extra delay until this returns false */
int VbAudioLooping(VbAudioContext* audio);

/* Caller should call this prior to booting */
void VbAudioClose(VbAudioContext* audio);

#endif /* VBOOT_REFERENCE_VBOOT_AUDIO_H_ */

