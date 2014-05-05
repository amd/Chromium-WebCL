/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#ifndef VP9_RDP_H_
#define VP9_RDP_H_

#define WHT_UPSCALE_FACTOR 2

void vp9_reconstruct_iwalsh_1_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_reconstruct_iwalsh_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_reconstruct_4x4_1_c_recon(int16_t input, uint8_t *dest, int dest_stride);

void vp_reconstruct_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp_reconstruct_8x8_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp_reconstruct_8x8_10_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp_reconstruct_8x8_1_c_recon(int16_t input, uint8_t *dest, int dest_stride);

void vp_reconstruct_16x16_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp_reconstruct_16x16_10_c_recon(int16_t *input, uint8_t *dest, int dest_stride);

void vp_reconstruct_16x16_1_c_recon(int16_t input, uint8_t *dest, int dest_stride);

void vp9_reconstruct_32x32_c_recon(int16_t *input, uint8_t *dest, int dest_stride, int eob);

#endif
