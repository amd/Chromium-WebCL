/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/* This optimisation includes: memory cache, float_filter, saturate, fmad */

#define SUBPEL_BITS 4
#define SUBPEL_SHIFTS 16
#define SUBPEL_TAPS 8

typedef uchar   uint8_t;
typedef short   int16_t;

typedef struct inter_pred_param_for_gpu {
  int src_stride;
  int filter_x_mv;
  int filter_y_mv;
}INTER_PRED_PARAM_GPU;

typedef struct inter_index_param {
    int pred_mode;
    int buf_offset;
    int dst_offset;
    int dst_stride;
    int pre_stride;
    int src_num;
    int w;
    int h;
    int sub_x;
    int sub_y;
}INTER_INDEX_PARAM_GPU;

__constant const float filter_bits_f = 128.0f;

/* The filters' array including 4 different filter' types */
__constant const int16_t vp9_sub_pel_filters[512] = {
  // Lagrangian interpolation filter
   0, 0,   0, 128,   0,   0, 0,  0,
   0, 1,  -5, 126,   8,  -3, 1,  0,
  -1, 3, -10, 122,  18,  -6, 2,  0,
  -1, 4, -13, 118,  27,  -9, 3, -1,
  -1, 4, -16, 112,  37, -11, 4, -1,
  -1, 5, -18, 105,  48, -14, 4, -1,
  -1, 5, -19,  97,  58, -16, 5, -1,
  -1, 6, -19,  88,  68, -18, 5, -1,
  -1, 6, -19,  78,  78, -19, 6, -1,
  -1, 5, -18,  68,  88, -19, 6, -1,
  -1, 5, -16,  58,  97, -19, 5, -1,
  -1, 4, -14,  48, 105, -18, 5, -1,
  -1, 4, -11,  37, 112, -16, 4, -1,
  -1, 3,  -9,  27, 118, -13, 4, -1,
   0, 2,  -6,  18, 122, -10, 3, -1,
   0, 1,  -3,   8, 126,  -5, 1,  0,
  // Freqmultiplier = 0.5
   0,  0,  0, 128,  0,  0,  0,  0,
  -3, -1, 32,  64, 38,  1, -3,  0,
  -2, -2, 29,  63, 41,  2, -3,  0,
  -2, -2, 26,  63, 43,  4, -4,  0,
  -2, -3, 24,  62, 46,  5, -4,  0,
  -2, -3, 21,  60, 49,  7, -4,  0,
  -1, -4, 18,  59, 51,  9, -4,  0,
  -1, -4, 16,  57, 53, 12, -4, -1,
  -1, -4, 14,  55, 55, 14, -4, -1,
  -1, -4, 12,  53, 57, 16, -4, -1,
   0, -4,  9,  51, 59, 18, -4, -1,
   0, -4,  7,  49, 60, 21, -3, -2,
   0, -4,  5,  46, 62, 24, -3, -2,
   0, -4,  4,  43, 63, 26, -2, -2,
   0, -3,  2,  41, 63, 29, -2, -2,
   0, -3,  1,  38, 64, 32, -1, -3,
  // DCT based filter
   0,  0,   0, 128,   0,   0,  0,  0,
  -1,  3,  -7, 127,   8,  -3,  1,  0,
  -2,  5, -13, 125,  17,  -6,  3, -1,
  -3,  7, -17, 121,  27, -10,  5, -2,
  -4,  9, -20, 115,  37, -13,  6, -2,
  -4, 10, -23, 108,  48, -16,  8, -3,
  -4, 10, -24, 100,  59, -19,  9, -3,
  -4, 11, -24,  90,  70, -21, 10, -4,
  -4, 11, -23,  80,  80, -23, 11, -4,
  -4, 10, -21,  70,  90, -24, 11, -4,
  -3,  9, -19,  59, 100, -24, 10, -4,
  -3,  8, -16,  48, 108, -23, 10, -4,
  -2,  6, -13,  37, 115, -20,  9, -4,
  -2,  5, -10,  27, 121, -17,  7, -3,
  -1,  3,  -6,  17, 125, -13,  5, -2,
   0,  1,  -3,   8, 127,  -7,  3, -1,
  // Blinear filter
   0, 0, 0, 128,   0, 0, 0, 0,
   0, 0, 0, 120,   8, 0, 0, 0,
   0, 0, 0, 112,  16, 0, 0, 0,
   0, 0, 0, 104,  24, 0, 0, 0,
   0, 0, 0,  96,  32, 0, 0, 0,
   0, 0, 0,  88,  40, 0, 0, 0,
   0, 0, 0,  80,  48, 0, 0, 0,
   0, 0, 0,  72,  56, 0, 0, 0,
   0, 0, 0,  64,  64, 0, 0, 0,
   0, 0, 0,  56,  72, 0, 0, 0,
   0, 0, 0,  48,  80, 0, 0, 0,
   0, 0, 0,  40,  88, 0, 0, 0,
   0, 0, 0,  32,  96, 0, 0, 0,
   0, 0, 0,  24, 104, 0, 0, 0,
   0, 0, 0,  16, 112, 0, 0, 0,
   0, 0, 0,   8, 120, 0, 0, 0
};

void vp9_convolve_copy_4x4(__global const uchar *src,
                           int src_stride,
                           __global uchar4 *dst) {
   uchar4 d[4];
   int s2 = src_stride << 1;
   int s3 = s2 + src_stride;
   int d1 = src_stride >> 2;
   int d2 = src_stride >> 1;
   int d3 = s3 >> 2;

   d[0] = vload4(0, src);
   d[1] = vload4(0, (src + src_stride));
   d[2] = vload4(0, (src + s2));
   d[3] = vload4(0, (src + s3));

   dst[0] = d[0];
   dst[d1] = d[1];
   dst[d2] = d[2];
   dst[d3] = d[3];
}

void vp9_convolve_horiz_4x4_only(__global const uchar *src,
                                 int src_stride,
                                 __global uchar4 *dst,
                                 __constant const short8 * filter_x0) {
  int a;
  float4 sum;
  float8 src1;
  float4 src2;
  float8 filter_val;

  __private float tt[44];
  __private float vv[16];

  __private float4 *vt = (__private float4*)tt;
  filter_val = (convert_float8(filter_x0[0])) / filter_bits_f;

  for (a = 0; a < 4; a++) {
    src1 = convert_float8(vload8(0, src));
    src2 = convert_float4(vload4(0, src + 8));

    tt[a] = (src1.s0);
    tt[(1 << 2) + a] = (src1.s1);
    tt[(2 << 2) + a] = (src1.s2);
    tt[(3 << 2) + a] = (src1.s3);
    tt[(4 << 2) + a] = (src1.s4);
    tt[(5 << 2) + a] = (src1.s5);
    tt[(6 << 2) + a] = (src1.s6);
    tt[(7 << 2) + a] = (src1.s7);
    tt[(8 << 2) + a] = (src2.s0);
    tt[(9 << 2) + a] = (src2.s1);
    tt[(10 << 2) + a] = (src2.s2);

    src += src_stride;
  }

  for ( a = 0; a < 4; a++) {
   // dst line 0
   sum = mad(vt[0], filter_val.s0,
         mad(vt[1], filter_val.s1,
         mad(vt[2], filter_val.s2,
         mad(vt[3], filter_val.s3,
         mad(vt[4], filter_val.s4,
         mad(vt[5], filter_val.s5,
         mad(vt[6], filter_val.s6,
         mad(vt[7], filter_val.s7,
         0.5f))))))));

    vv[0 + a] = sum.x;
    vv[4 + a] = sum.y;
    vv[8 + a] = sum.z;
    vv[12  + a] = sum.w;

    vt += 1;
  }

  src_stride >>= 2;
  __private float4 *rr = (__private float4*)vv;

  dst[0] = convert_uchar4_sat(rr[0]);
  dst += src_stride;

  dst[0] = convert_uchar4_sat(rr[1]);
  dst += src_stride;

  dst[0] = convert_uchar4_sat(rr[2]);
  dst += src_stride;

  dst[0] = convert_uchar4_sat(rr[3]);
}

void vp9_convolve_vert_4x4_only(__global const uchar *src,
                                __global uchar4 *dst, int stride,
                                __constant const short8 * filter_y0) {
  float8 filter_val;
  float4 sum0, sum1, sum2, sum3;
  float4 src_f[11];

  uchar4 s[11];
#define LD(n) s[n] = vload4(0, src + stride * n)
  LD(0);
  LD(1);
  LD(2);
  LD(3);
  LD(4);
  LD(5);
  LD(6);
  LD(7);
  LD(8);
  LD(9);
  LD(10);
#undef LD

#define ST(n) src_f[n] = convert_float4(s[n])
  ST(0);
  ST(1);
  ST(2);
  ST(3);
  ST(4);
  ST(5);
  ST(6);
  ST(7);
  ST(8);
  ST(9);
  ST(10);
#undef ST

  filter_val = (convert_float8(filter_y0[0])) / filter_bits_f;

  // Float filter
  sum0 = mad(src_f[0], filter_val.s0,
         mad(src_f[1], filter_val.s1,
         mad(src_f[2], filter_val.s2,
         mad(src_f[3], filter_val.s3,
         mad(src_f[4], filter_val.s4,
         mad(src_f[5], filter_val.s5,
         mad(src_f[6], filter_val.s6,
         mad(src_f[7], filter_val.s7,
         0.5f))))))));

  sum1 = mad(src_f[1], filter_val.s0,
         mad(src_f[2], filter_val.s1,
         mad(src_f[3], filter_val.s2,
         mad(src_f[4], filter_val.s3,
         mad(src_f[5], filter_val.s4,
         mad(src_f[6], filter_val.s5,
         mad(src_f[7], filter_val.s6,
         mad(src_f[8], filter_val.s7,
         0.5f))))))));

  sum2 = mad(src_f[2], filter_val.s0,
         mad(src_f[3], filter_val.s1,
         mad(src_f[4], filter_val.s2,
         mad(src_f[5], filter_val.s3,
         mad(src_f[6], filter_val.s4,
         mad(src_f[7], filter_val.s5,
         mad(src_f[8], filter_val.s6,
         mad(src_f[9], filter_val.s7,
         0.5f))))))));

  sum3 = mad(src_f[3], filter_val.s0,
         mad(src_f[4], filter_val.s1,
         mad(src_f[5], filter_val.s2,
         mad(src_f[6], filter_val.s3,
         mad(src_f[7], filter_val.s4,
         mad(src_f[8], filter_val.s5,
         mad(src_f[9], filter_val.s6,
         mad(src_f[10], filter_val.s7,
         0.5f))))))));

  stride >>= 2;

  dst[0] = convert_uchar4_sat(sum0);
  dst += stride;

  dst[0] = convert_uchar4_sat(sum1);
  dst += stride;

  dst[0] = convert_uchar4_sat(sum2);
  dst += stride;

  dst[0] = convert_uchar4_sat(sum3);

}

void vp9_convolve_horiz_ocl_4x11(__global const uchar *src,
                                 int src_stride, __private float4 *dst,
                                 __constant const short8 * filter_x0) {
  float8 src1;
  int nOffsetSrc;
  float8 filter_val;
  filter_val = (convert_float8(filter_x0[0])) / filter_bits_f;

  for (nOffsetSrc = 0; nOffsetSrc < 11; ++nOffsetSrc) {
    dst[0] = convert_float4(vload4(0,src));
    src1 = convert_float8(vload8(0,src + 4));

    //calculation
    dst[0].s0 = mad(dst[0].s0 , filter_val.s0,
                mad(dst[0].s1, filter_val.s1,
                mad(dst[0].s2, filter_val.s2,
                mad(dst[0].s3, filter_val.s3,
                mad(src1.s0, filter_val.s4,
                mad(src1.s1, filter_val.s5,
                mad(src1.s2, filter_val.s6,
                mad(src1.s3, filter_val.s7,
                0.5f))))))));

    dst[0].s1 = mad(dst[0].s1 , filter_val.s0,
                mad(dst[0].s2, filter_val.s1,
                mad(dst[0].s3, filter_val.s2,
                mad(src1.s0, filter_val.s3,
                mad(src1.s1, filter_val.s4,
                mad(src1.s2, filter_val.s5,
                mad(src1.s3, filter_val.s6,
                mad(src1.s4, filter_val.s7,
                0.5f))))))));

    dst[0].s2 = mad(dst[0].s2 , filter_val.s0,
                mad(dst[0].s3, filter_val.s1,
                mad(src1.s0, filter_val.s2,
                mad(src1.s1, filter_val.s3,
                mad(src1.s2, filter_val.s4,
                mad(src1.s3, filter_val.s5,
                mad(src1.s4, filter_val.s6,
                mad(src1.s5, filter_val.s7,
                0.5f))))))));

    dst[0].s3 = mad(dst[0].s3 , filter_val.s0,
                mad(src1.s0, filter_val.s1,
                mad(src1.s1, filter_val.s2,
                mad(src1.s2, filter_val.s3,
                mad(src1.s3, filter_val.s4,
                mad(src1.s4, filter_val.s5,
                mad(src1.s5, filter_val.s6,
                mad(src1.s6, filter_val.s7,
                0.5f))))))));

    dst[0] = convert_float4(convert_uchar4_sat(dst[0]));

    src += (src_stride);
    dst += 1;
  }
}

/**
 * vp9_convolve_vert_ocl_4x4,
 * do convolution in vertical pass
 * @src [in] input source
 * @dst  [out] ouput results
 * @dst_stride [in] size of one stride in destination
 * @filter_y0 [in] start position of one filter row
 * @return void
 */
void vp9_convolve_vert_ocl_4x4(__private const float4 *src,
                               __global uchar4 *dst, int dst_stride,
                               __constant const short8 * filter_y0) {
  uint r;
  float4 sum;
  float8 filter_val;

  filter_val = (convert_float8(filter_y0[0])) / filter_bits_f;

  // Float filter
  for( r = 0; r < 4; r++) {
    // dst line 0
    sum = mad(src[0], filter_val.s0,
          mad(src[1], filter_val.s1,
          mad(src[2], filter_val.s2,
          mad(src[3], filter_val.s3,
          mad(src[4], filter_val.s4,
          mad(src[5], filter_val.s5,
          mad(src[6], filter_val.s6,
          mad(src[7], filter_val.s7,
          0.5f))))))));

    dst[0] = convert_uchar4_sat(sum);

    src += 1;
    dst += dst_stride;
  }
}

void vp9_convolve_vert_ocl_4x4_avg(__private const float4 *src,
                               __global uchar4 *dst, int dst_stride,
                               __constant const short8 * filter_y0) {
  uint r;
  float4 sum;
  float8 filter_val;

  filter_val = (convert_float8(filter_y0[0])) / filter_bits_f;

  // Float filter
  for( r = 0; r < 4; r++) {
    // dst line 0
    sum = mad(src[0], filter_val.s0,
          mad(src[1], filter_val.s1,
          mad(src[2], filter_val.s2,
          mad(src[3], filter_val.s3,
          mad(src[4], filter_val.s4,
          mad(src[5], filter_val.s5,
          mad(src[6], filter_val.s6,
          mad(src[7], filter_val.s7,
          0.5f))))))));

    dst[0] = convert_uchar4((convert_ushort4(convert_uchar4_sat(sum)) +
                             convert_ushort4(dst[0]) + 1) >> 1);

     src += 1;
     dst += dst_stride;
  }
}

/**
 * inter_pred_calcu,
 * the kernel of the inter prediction calculation
 * @buffer_pool [in] input sources
 * @new_buffer  [out] output results
 * @inter_pred_param [in] block paramenters required to be calculated
 * @param_num [in] the block number which the thread index
 * @x_mv [in] the offset which the thread will start to process
 * @buffer_size [in] one frame size
 * @counts_8x8 [in] the real number of thread
 * @return void
 */
__kernel 
__attribute__((reqd_work_group_size(64, 1, 1)))
void inter_pred_calcu(
                  __global uint8_t *buffer_pool_write,
                  __global int *dst_x_mv,
                  __global const INTER_PRED_PARAM_GPU *inter_pred_param,
                  __global const int *param_num,
                  __global const int *x_mv,
                  __constant const int *case_count,
                  __constant const int *one_case_interval_count,
                  __constant const int *all_of_block_count,
                  __global uint8_t *buffer_pool_read) {
  uint gIdx = get_global_id(0);
  if (gIdx >= all_of_block_count[0])
    return ;

  int real_position;
  __global int * nBlkPrmPtr;
  __private float temp_buffer[44];

  int case0 = case_count[0];
  int case1 = case_count[1];
  int case2 = case_count[2];

  if (gIdx <case0) {
    real_position = gIdx;
    nBlkPrmPtr =
        (__global int *) (inter_pred_param + param_num[real_position]);

    vp9_convolve_copy_4x4(buffer_pool_read + x_mv[real_position],
                          nBlkPrmPtr[0],
                          (__global uchar4 *)(buffer_pool_write +
                                              dst_x_mv[real_position]));
  } else if (gIdx >= case0 && gIdx < case1 + case0) {
    real_position = 1 * one_case_interval_count[0] + gIdx - case0;
    nBlkPrmPtr =
        (__global int *) (inter_pred_param + param_num[real_position]);

    vp9_convolve_vert_4x4_only(buffer_pool_read + x_mv[real_position],
                               (__global uchar4 *)(buffer_pool_write +
                                                   dst_x_mv[real_position]),
                               nBlkPrmPtr[0],
                               (__constant const short8*)(vp9_sub_pel_filters +
                                                          nBlkPrmPtr[2]));
  } else if (gIdx >=  case1 + case0 && gIdx <  case1 + case0 + case2) {
    real_position = 2 * one_case_interval_count[0] + gIdx - case0 - case1;
    nBlkPrmPtr =
        (__global int *) (inter_pred_param + param_num[real_position]);

    vp9_convolve_horiz_4x4_only(buffer_pool_read + x_mv[real_position],
                                nBlkPrmPtr[0],
                                (__global uchar4 *)(buffer_pool_write +
                                                    dst_x_mv[real_position]),
                                (__constant const short8*)(vp9_sub_pel_filters +
                                                           nBlkPrmPtr[1]));
  } else {
    real_position =
        3 * one_case_interval_count[0] + gIdx - case0 - case1 - case2;
    nBlkPrmPtr =
        (__global int *) (inter_pred_param + param_num[real_position]);

    vp9_convolve_horiz_ocl_4x11((buffer_pool_read + x_mv[real_position]),
                                nBlkPrmPtr[0], (float4 *)temp_buffer,
                                (__constant const short8*)(vp9_sub_pel_filters +
                                                           nBlkPrmPtr[1]));

    vp9_convolve_vert_ocl_4x4((__private float4*)(temp_buffer),
                              (__global uchar4*)(buffer_pool_write +
                                                 dst_x_mv[real_position]),
                              (nBlkPrmPtr[0] >> 2),
                              (__constant const short8*)(vp9_sub_pel_filters +
                                                         nBlkPrmPtr[2]));
  }
}

__kernel void inter_pred_index(
                  __global INTER_INDEX_PARAM_GPU *index_param,
                  __constant const int *new_fb_idx,
                  __constant const int *buffer_size,
                  __constant const int *gpu_block_count,
                  __constant const int *index_case_mode_offset,
                  __constant const int *tile_param_count_gpu_offset,
                  __constant const int *all_b_count,
                  __constant const int *tile_count,
                  __global int *dst_x_mv,
                  __global int *param_num,
                  __global int *x_mv,
                  __global int *case_count) {
  uint gIdx = get_global_id(0);
  if (gIdx >= all_b_count[0])
    return ;

  int i, j;
  int tile_num;
  int block_num_pre;
  int tile_block_count = 0;

  for (tile_num = 0; tile_num < tile_count[0]; ++tile_num) {
    if (gIdx >= tile_block_count && gIdx < tile_block_count + gpu_block_count[tile_num]) {
      block_num_pre = gIdx - tile_block_count;

      __global INTER_INDEX_PARAM_GPU *index_param_pre =
          index_param + tile_param_count_gpu_offset[tile_num] + block_num_pre;

      int w = index_param_pre->w;
      int h = index_param_pre->h;

      int index_wide = h * w;

      int case_num =
          atomic_add(&case_count[index_param_pre->pred_mode >> 1],
                     index_wide);

      int tile_param_case_offset =
            tile_param_count_gpu_offset[tile_num] + block_num_pre;

      int index_xmv_offset =
          index_param_pre->buf_offset +
          index_param_pre->src_num * buffer_size[0] -
          3 * index_param_pre->sub_y * index_param_pre->pre_stride -
          3 * index_param_pre->sub_x;

      int dst_index_xmv_offset =
          new_fb_idx[0] * buffer_size[0] + index_param_pre->dst_offset;

      int index_case_mode =
          index_case_mode_offset[index_param_pre->pred_mode >> 1] + case_num;

      int pre_j_mv = index_param_pre->pre_stride << 2;
      int dst_j_mv = index_param_pre->dst_stride << 2;

      int16 off16 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
      int16 param_v16 = {tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset,
                         tile_param_case_offset, tile_param_case_offset};

      for (j = 0; j < h; ++j) {
        if (w >= 16) {
          for (i = 0; i < w; i+=16) {
            __global int16 *p16 = (__global int16 *)&param_num[index_case_mode];
            p16[0] = param_v16;

            int16 tmp16 = ((off16+i) << 2) + index_xmv_offset;
            p16 = (__global int16 *)&x_mv[index_case_mode];
            p16[0] = tmp16;

            tmp16 = ((off16 + i) << 2) + dst_index_xmv_offset;
            p16 = (__global int16 *)&dst_x_mv[index_case_mode];
            p16[0] = tmp16;

            index_case_mode += 16;
          }
        } else if (w >= 8) {
          for (i = 0; i < w; i+=8) {
            __global int8 *p8 = (__global int8 *)&param_num[index_case_mode];
            p8[0] = param_v16.s01234567;

            int8 tmp8 = ((off16.s01234567+i) << 2) + index_xmv_offset;
            p8 = (__global int8 *)&x_mv[index_case_mode];
            p8[0] = tmp8;

            tmp8 = ((off16.s01234567 + i) << 2) + dst_index_xmv_offset;
            p8 = (__global int8 *)&dst_x_mv[index_case_mode];
            p8[0] = tmp8;

            index_case_mode += 8;
          }
        } else if (w >= 4) {
          for (i = 0; i < w; i+=4) {
            __global int4 *p4 = (__global int4 *)&param_num[index_case_mode];
            p4[0] = param_v16.s0123;

            int4 tmp4 = ((off16.s0123+i) << 2) + index_xmv_offset;
            p4 = (__global int4 *)&x_mv[index_case_mode];
            p4[0] = tmp4;

            tmp4 = ((off16.s0123 + i) << 2) + dst_index_xmv_offset;
            p4 = (__global int4 *)&dst_x_mv[index_case_mode];
            p4[0] = tmp4;

            index_case_mode += 4;
          }
        } else if (w >= 2) {
          for (i = 0; i < w; i+=2) {
            __global int2 *p2 = (__global int2 *)&param_num[index_case_mode];
            p2[0] = param_v16.s01;

            int2 tmp2 = ((off16.s01+i) << 2) + index_xmv_offset;
            p2 = (__global int2 *)&x_mv[index_case_mode];
            p2[0] = tmp2;

            tmp2 = ((off16.s01 + i) << 2) + dst_index_xmv_offset;
            p2 = (__global int2 *)&dst_x_mv[index_case_mode];
            p2[0] = tmp2;

            index_case_mode += 2;
          }
        } else {
          for (i = 0; i < w; ++i) {
            param_num[index_case_mode] = tile_param_case_offset;
            x_mv[index_case_mode] = index_xmv_offset + (i << 2);
            dst_x_mv[index_case_mode] = dst_index_xmv_offset+ (i << 2);
            index_case_mode++;
          }
        }
        index_xmv_offset += pre_j_mv;
        dst_index_xmv_offset += dst_j_mv;
      }

      break ;
    }
    tile_block_count += gpu_block_count[tile_num];
  }
}

__kernel void update_buffer_pool(
                  __global uint8_t *buffer_pool_read,
                  __global uint8_t *buffer_pool_write,
                  const uint buffer_offset) {
  uint gIdx = get_global_id(0);
  uint pre_offset = buffer_offset + (gIdx << 2);

  __global float *src = (__global float *)(buffer_pool_read + pre_offset);
  __global float *dst = (__global float *)(buffer_pool_write + pre_offset);

  dst[0] = src[0];
}
