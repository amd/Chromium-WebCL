/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vpx/vpx_integer.h"
#include "vp9/common/vp9_common.h"
#include "vp9/decoder/vp9_rdp.h"

void vp9_reconstruct_iwalsh_1_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i;
  int a1, e1;
  int16_t tmp[4];
  int16_t *ip = input;
  int16_t *op = tmp;

  a1 = ip[0] >> WHT_UPSCALE_FACTOR;
  e1 = a1 >> 1;
  a1 -= e1;
  op[0] = a1;
  op[1] = op[2] = op[3] = e1;

  ip = tmp;
  for (i = 0; i < 4; i++) {
    e1 = ip[0] >> 1;
    a1 = ip[0] - e1;
    dest[dest_stride * 0] = clip_pixel(dest[dest_stride * 0] + a1);
    dest[dest_stride * 1] = clip_pixel(dest[dest_stride * 1] + e1);
    dest[dest_stride * 2] = clip_pixel(dest[dest_stride * 2] + e1);
    dest[dest_stride * 3] = clip_pixel(dest[dest_stride * 3] + e1);
    ip++;
    dest++;
  }
  
}

void vp9_reconstruct_iwalsh_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i;
  int a1, b1, c1, d1, e1;

  for (i = 0; i < 4; i++) {
    a1 = input[4 * 0];
    c1 = input[4 * 1];
    d1 = input[4 * 2];
    b1 = input[4 * 3];
    a1 += c1;
    d1 -= b1;
    e1 = (a1 - d1) >> 1;
    b1 = e1 - b1;
    c1 = e1 - c1;
    a1 -= b1;
    d1 += c1;
    dest[dest_stride * 0] = clip_pixel(dest[dest_stride * 0] + a1);
    dest[dest_stride * 1] = clip_pixel(dest[dest_stride * 1] + b1);
    dest[dest_stride * 2] = clip_pixel(dest[dest_stride * 2] + c1);
    dest[dest_stride * 3] = clip_pixel(dest[dest_stride * 3] + d1);

    input++;
    dest++;
  }
  
}

void vp9_reconstruct_4x4_1_c_recon(int16_t input, uint8_t *dest, int dest_stride) {
  int i, a1;

  a1 = ROUND_POWER_OF_TWO(input, 4);

  for (i = 0; i < 4; i++) {
    dest[0] = clip_pixel(dest[0] + a1);
    dest[1] = clip_pixel(dest[1] + a1);
    dest[2] = clip_pixel(dest[2] + a1);
    dest[3] = clip_pixel(dest[3] + a1);
    dest += dest_stride;
  }
}
  
void vp_reconstruct_4x4_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i, j;

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j)
      dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 4)
                                      + dest[j * dest_stride + i]);
  }
}

void vp_reconstruct_8x8_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i, j;

  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 8; ++j)
      dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 5)
                                      + dest[j * dest_stride + i]);
  }
}

void vp_reconstruct_8x8_10_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i, j;

  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 8; ++j)
      dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 5)
                                      + dest[j * dest_stride + i]);
  }
}

void vp_reconstruct_8x8_1_c_recon(int16_t input, uint8_t *dest, int dest_stride) {
  int i, j, a1;

  a1 = ROUND_POWER_OF_TWO(input, 5);
  for (j = 0; j < 8; ++j) {
    for (i = 0; i < 8; ++i)
      dest[i] = clip_pixel(dest[i] + a1);
    dest += dest_stride;
  }
}

void vp_reconstruct_16x16_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i, j;

  for (i = 0; i < 16; ++i) {
    for (j = 0; j < 16; ++j)
      dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 6)
                                      + dest[j * dest_stride + i]);
  }
}

void vp_reconstruct_16x16_10_c_recon(int16_t *input, uint8_t *dest, int dest_stride) {
  int i, j;

  for (i = 0; i < 16; ++i) {
    for (j = 0; j < 16; ++j)
      dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 6)
                                      + dest[j * dest_stride + i]);
  }
}

void vp_reconstruct_16x16_1_c_recon(int16_t input, uint8_t *dest, int dest_stride) {
  int i, j, a1;

  a1 = ROUND_POWER_OF_TWO(input, 6);
  for (j = 0; j < 16; ++j) {
    for (i = 0; i < 16; ++i)
      dest[i] = clip_pixel(dest[i] + a1);
    dest += dest_stride;
  }
}

void vp9_reconstruct_32x32_c_recon(int16_t *input, uint8_t *dest, int dest_stride, int eob) {
  int i, j;

  if (eob == 1) {
    for (i = 0; i < 32; i++) {
      for (j = 0; j < 32; j++)
        dest[i] = clip_pixel(input[0] + dest[i]);
      dest += dest_stride;
    }
  } else {
    for (i = 0; i < 32; ++i) {
      for (j = 0; j < 32; ++j)
        dest[j * dest_stride + i] = clip_pixel(ROUND_POWER_OF_TWO(input[j], 6)
                                    + dest[j * dest_stride + i]);
    }
  }
}

