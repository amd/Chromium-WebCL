/* vp9 loop filter ocl kernel
 * each thread do one super block(64*64)
 */
#define TX_SIZES 4
#define SIMD_WIDTH 16
#define MAX_LOOP_FILTER 63
#define MAX_SEGMENTS     8
#define MAX_REF_FRAMES  4
#define MAX_REF_LF_DELTAS       4
#define MAX_MODE_LF_DELTAS      2
#define MB_MODE_COUNT 14
#define MI_BLOCK_SIZE 8

#define ROUND_POWER_OF_TWO(value, n) \
    (((value) + (1 << ((n) - 1))) >> (n))

typedef uchar   uint8_t;
typedef short   int16_t;
typedef ushort  uint16_t;
//typedef ulong   uint64_t;
//typedef uint   uint64_t;
typedef uint   uint32_t;
typedef char   int8_t;


typedef enum {
  PLANE_TYPE_Y  = 0,
  PLANE_TYPE_UV = 1,
  PLANE_TYPES
} PLANE_TYPE;

typedef struct {
  uint32_t left_y[TX_SIZES];
  uint32_t above_y[TX_SIZES];
  uint32_t int_4x4_y;
  uint16_t left_uv[TX_SIZES];
  uint16_t above_uv[TX_SIZES];
  uint16_t int_4x4_uv;
  uint8_t lfl_y[64];
  uint8_t lfl_uv[16];
} LOOP_FILTER_MASK_OCL;

typedef struct {
  uint8_t mblim[SIMD_WIDTH];
  uint8_t lim[SIMD_WIDTH];
  uint8_t hev_thr[SIMD_WIDTH];
} loop_filter_thresh_ocl;

typedef struct {
  loop_filter_thresh_ocl lfthr[MAX_LOOP_FILTER + 1];
  uint8_t lvl[MAX_SEGMENTS][MAX_REF_FRAMES][MAX_MODE_LF_DELTAS];
  uint8_t mode_lf_lut[MB_MODE_COUNT];
} loop_filter_info_n_ocl;

typedef struct loopfilter_param_ocl{
  int dst_offset;
  int dst_stride;

  int mi_row;
  int mi_rows;
} LOOPFILTER_PARAM_OCL;

/* functions used by kernel*/
int8_t signed_char_clamp(int t) {
  return (int8_t)clamp(t, -128, 127);
}

char4 signed_char_clamp_v4(int4 t) {
  return (convert_char4)(clamp(t, (int4)(-128,-128,-128,-128), (int4)(127,127,127,127)));
}


// should we apply any filter at all: 11111111 yes, 00000000 no
 int8_t filter_mask(uint8_t limit, uint8_t blimit,
                                 uint8_t p3, uint8_t p2,
                                 uint8_t p1, uint8_t p0,
                                 uint8_t q0, uint8_t q1,
                                 uint8_t q2, uint8_t q3) {
  int8_t mask = 0;
  mask |= (abs(p3 - p2) > limit) * -1;
  mask |= (abs(p2 - p1) > limit) * -1;
  mask |= (abs(p1 - p0) > limit) * -1;
  mask |= (abs(q1 - q0) > limit) * -1;
  mask |= (abs(q2 - q1) > limit) * -1;
  mask |= (abs(q3 - q2) > limit) * -1;
  mask |= (abs(p0 - q0) * 2 + abs(p1 - q1) / 2  > blimit) * -1;
  return ~mask;
}



 int8_t flat_mask4(uint8_t thresh,
                                uint8_t p3, uint8_t p2,
                                uint8_t p1, uint8_t p0,
                                uint8_t q0, uint8_t q1,
                                uint8_t q2, uint8_t q3) {
  int8_t mask = 0;
  mask |= (abs(p1 - p0) > thresh) * -1;
  mask |= (abs(q1 - q0) > thresh) * -1;
  mask |= (abs(p2 - p0) > thresh) * -1;
  mask |= (abs(q2 - q0) > thresh) * -1;
  mask |= (abs(p3 - p0) > thresh) * -1;
  mask |= (abs(q3 - q0) > thresh) * -1;
  return ~mask;
}

 

 int8_t flat_mask5(uint8_t thresh,
                                uint8_t p4, uint8_t p3,
                                uint8_t p2, uint8_t p1,
                                uint8_t p0, uint8_t q0,
                                uint8_t q1, uint8_t q2,
                                uint8_t q3, uint8_t q4) {
  int8_t mask = ~flat_mask4(thresh, p3, p2, p1, p0, q0, q1, q2, q3);
  mask |= (abs(p4 - p0) > thresh) * -1;
  mask |= (abs(q4 - q0) > thresh) * -1;
  return ~mask;
}

 

// is there high edge variance internal edge: 11111111 yes, 00000000 no
 int8_t hev_mask(uint8_t thresh, uint8_t p1, uint8_t p0,
                              uint8_t q0, uint8_t q1) {
  int8_t hev = 0;
  hev  |= (abs(p1 - p0) > thresh) * -1;
  hev  |= (abs(q1 - q0) > thresh) * -1;
  return hev;
}



#define FILTER_4(mask,thresh, op1,\
                          op0,  oq0,  oq1) {\
  int8_t filter1, filter2;\
  const int8_t ps1 = (int8_t) op1 ^ 0x80;\
  const int8_t ps0 = (int8_t) op0 ^ 0x80;\
  const int8_t qs0 = (int8_t) oq0 ^ 0x80;\
  const int8_t qs1 = (int8_t) oq1 ^ 0x80;\
  const uint8_t hev = hev_mask(thresh, op1, op0, oq0, oq1);\
  int8_t filter = signed_char_clamp(ps1 - qs1) & hev;\
  filter = signed_char_clamp(filter + 3 * (qs0 - ps0)) & mask;\
  filter1 = signed_char_clamp(filter + 4) >> 3;\
  filter2 = signed_char_clamp(filter + 3) >> 3;\
  oq0 = signed_char_clamp(qs0 - filter1) ^ 0x80;\
  op0 = signed_char_clamp(ps0 + filter2) ^ 0x80;\
  filter = ROUND_POWER_OF_TWO(filter1, 1) & ~hev;\
  oq1 = signed_char_clamp(qs1 - filter) ^ 0x80;\
  op1 = signed_char_clamp(ps1 + filter) ^ 0x80;\
}


#define FILTER_8(mask, thresh, flat, op3,  op2, op1, op0, oq0, oq1, oq2, oq3) {\
  if (flat && mask) {\
    op2 = ROUND_POWER_OF_TWO(op3 + op3 + op3 + 2 * op2 + op1 + op0 + oq0, 3);\
    op1 = ROUND_POWER_OF_TWO(op3 + op3 + op2 + 2 * op1 + op0 + oq0 + oq1, 3);\
    op0 = ROUND_POWER_OF_TWO(op3 + op2 + op1 + 2 * op0 + oq0 + oq1 + oq2, 3);\
    oq0 = ROUND_POWER_OF_TWO(op2 + op1 + op0 + 2 * oq0 + oq1 + oq2 + oq3, 3);\
    oq1 = ROUND_POWER_OF_TWO(op1 + op0 + oq0 + 2 * oq1 + oq2 + oq3 + oq3, 3);\
    oq2 = ROUND_POWER_OF_TWO(op0 + oq0 + oq1 + 2 * oq2 + oq3 + oq3 + oq3, 3);\
  } else {\
    FILTER_4(mask, thresh, op1,  op0, oq0, oq1);\
  }\
}

#define FILTER_16(mask,thresh,flat,  flat2, p7, p6,p5,  p4,\
                  p3,  p2,p1,  p0, q0,  q1,  q2,  q3, q4,q5,q6,q7) {\
  if (flat2 && flat && mask) {\
    p6 = ROUND_POWER_OF_TWO(p7 * 7 + p6 * 2 + p5 + p4 + p3 + p2 + p1 + p0 +\
                              q0, 4);\
    p5 = ROUND_POWER_OF_TWO(p7 * 6 + p6 + p5 * 2 + p4 + p3 + p2 + p1 + p0 +\
                              q0 + q1, 4);\
    p4 = ROUND_POWER_OF_TWO(p7 * 5 + p6 + p5 + p4 * 2 + p3 + p2 + p1 + p0 +\
                              q0 + q1 + q2, 4);\
    p3 = ROUND_POWER_OF_TWO(p7 * 4 + p6 + p5 + p4 + p3 * 2 + p2 + p1 + p0 +\
                              q0 + q1 + q2 + q3, 4);\
    p2 = ROUND_POWER_OF_TWO(p7 * 3 + p6 + p5 + p4 + p3 + p2 * 2 + p1 + p0 +\
                              q0 + q1 + q2 + q3 + q4, 4);\
    p1 = ROUND_POWER_OF_TWO(p7 * 2 + p6 + p5 + p4 + p3 + p2 + p1 * 2 + p0 +\
                              q0 + q1 + q2 + q3 + q4 + q5, 4);\
    p0 = ROUND_POWER_OF_TWO(p7 + p6 + p5 + p4 + p3 + p2 + p1 + p0 * 2 +\
                              q0 + q1 + q2 + q3 + q4 + q5 + q6, 4);\
    q0 = ROUND_POWER_OF_TWO(p6 + p5 + p4 + p3 + p2 + p1 + p0 +\
                              q0 * 2 + q1 + q2 + q3 + q4 + q5 + q6 + q7, 4);\
    q1 = ROUND_POWER_OF_TWO(p5 + p4 + p3 + p2 + p1 + p0 +\
                              q0 + q1 * 2 + q2 + q3 + q4 + q5 + q6 + q7 * 2, 4);\
    q2 = ROUND_POWER_OF_TWO(p4 + p3 + p2 + p1 + p0 +\
                              q0 + q1 + q2 * 2 + q3 + q4 + q5 + q6 + q7 * 3, 4);\
    q3 = ROUND_POWER_OF_TWO(p3 + p2 + p1 + p0 +\
                              q0 + q1 + q2 + q3 * 2 + q4 + q5 + q6 + q7 * 4, 4);\
    q4 = ROUND_POWER_OF_TWO(p2 + p1 + p0 +\
                              q0 + q1 + q2 + q3 + q4 * 2 + q5 + q6 + q7 * 5, 4);\
    q5 = ROUND_POWER_OF_TWO(p1 + p0 +\
                              q0 + q1 + q2 + q3 + q4 + q5 * 2 + q6 + q7 * 6, 4);\
    q6 = ROUND_POWER_OF_TWO(p0 +\
                              q0 + q1 + q2 + q3 + q4 + q5 + q6 * 2 + q7 * 7, 4);\
  } else {\
    FILTER_8(mask, thresh, flat, p3, p2, p1, p0, q0, q1, q2, q3);\
  }\
}



 void filter4(int8_t mask, uint8_t thresh, __global uint8_t *op1,
                           __global uint8_t *op0, __global uint8_t *oq0, __global uint8_t *oq1) {
  int8_t filter1, filter2;

  const int8_t ps1 = (int8_t) *op1 ^ 0x80;
  const int8_t ps0 = (int8_t) *op0 ^ 0x80;
  const int8_t qs0 = (int8_t) *oq0 ^ 0x80;
  const int8_t qs1 = (int8_t) *oq1 ^ 0x80;
  const uint8_t hev = hev_mask(thresh, *op1, *op0, *oq0, *oq1);

  // add outer taps if we have high edge variance
  int8_t filter = signed_char_clamp(ps1 - qs1) & hev;

  // inner taps
  filter = signed_char_clamp(filter + 3 * (qs0 - ps0)) & mask;

  // save bottom 3 bits so that we round one side +4 and the other +3
  // if it equals 4 we'll set to adjust by -1 to account for the fact
  // we'd round 3 the other way
  filter1 = signed_char_clamp(filter + 4) >> 3;
  filter2 = signed_char_clamp(filter + 3) >> 3;

  *oq0 = signed_char_clamp(qs0 - filter1) ^ 0x80;
  *op0 = signed_char_clamp(ps0 + filter2) ^ 0x80;

  // outer tap adjustments
  filter = ROUND_POWER_OF_TWO(filter1, 1) & ~hev;

  *oq1 = signed_char_clamp(qs1 - filter) ^ 0x80;
  *op1 = signed_char_clamp(ps1 + filter) ^ 0x80;
}

void vp9_lpf_horizontal_4_c(__global uint8_t *s, int p /* pitch */,
                             __constant const uint8_t *blimit,  __constant const uint8_t *limit,
                             __constant const uint8_t *thresh, int count) {
  int i;

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < 8 * count; ++i) {
    const uint8_t p3 = s[-4 * p], p2 = s[-3 * p], p1 = s[-2 * p], p0 = s[-p];
    const uint8_t q0 = s[0 * p],  q1 = s[1 * p],  q2 = s[2 * p],  q3 = s[3 * p];
    const int8_t mask = filter_mask(*limit, *blimit,
                                    p3, p2, p1, p0, q0, q1, q2, q3);
    filter4(mask, *thresh, s - 2 * p, s - 1 * p, s, s + 1 * p);
    ++s;
  }
}

void vp9_lpf_horizontal_4_dual_c(__global uint8_t *s, int p,  __constant const uint8_t *blimit0,
                                  __constant const uint8_t *limit0,  __constant const uint8_t *thresh0,
                                  __constant const uint8_t *blimit1,  __constant const uint8_t *limit1,
                                  __constant const uint8_t *thresh1) {
  vp9_lpf_horizontal_4_c(s, p, blimit0, limit0, thresh0, 1);
  vp9_lpf_horizontal_4_c(s + 8, p, blimit1, limit1, thresh1, 1);
}

void vp9_lpf_vertical_4_c(__global uchar4 *s, int pitch,  __constant const uint8_t *blimit,
                           __constant const uint8_t *limit,  __constant const uint8_t *thresh,
                          int count) {
  int i;

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < 8 * count; ++i) {
    uchar4 sl = s[-1], sr = s[0];
    //const uint8_t p3 = sl.x, p2 = sl.y, p1 = sl.z, p0 = sl.w;
    //const uint8_t q0 = sr.x,  q1 = sr.y,  q2 = s[2],  q3 = s[3];
    const int8_t mask = filter_mask(*limit, *blimit,
                                    sl.x, sl.y, sl.z, sl.w, sr.x, sr.y, sr.z, sr.w);
   // filter4_recon(mask, *thresh,  s - 1, s);
    FILTER_4(mask, *thresh, sl.z, sl.w, sr.x, sr.y);
    s[-1] = sl;
    s[0] = sr;
    s += pitch;
  }
}

void vp9_lpf_vertical_4_dual_c(__global uchar4 *s, int pitch,  __constant const uint8_t *blimit0,
                                __constant const uint8_t *limit0,  __constant const uint8_t *thresh0,
                                __constant const uint8_t *blimit1,  __constant const uint8_t *limit1,
                                __constant const uint8_t *thresh1) {
  vp9_lpf_vertical_4_c(s, pitch, blimit0, limit0, thresh0, 1);
  vp9_lpf_vertical_4_c(s + 8 * pitch, pitch, blimit1, limit1,
                                  thresh1, 1);
}


 

void filter8(int8_t mask, uint8_t thresh, uint8_t flat,
                           __global uint8_t *op3, __global uint8_t *op2,
                           __global uint8_t *op1, __global uint8_t *op0,
                           __global uint8_t *oq0, __global uint8_t *oq1,
                           __global uint8_t *oq2, __global uint8_t *oq3) {
  if (flat && mask) {
    const uint8_t p3 = *op3, p2 = *op2, p1 = *op1, p0 = *op0;
    const uint8_t q0 = *oq0, q1 = *oq1, q2 = *oq2, q3 = *oq3;

    // 7-tap filter [1, 1, 1, 2, 1, 1, 1]
    *op2 = ROUND_POWER_OF_TWO(p3 + p3 + p3 + 2 * p2 + p1 + p0 + q0, 3);
    *op1 = ROUND_POWER_OF_TWO(p3 + p3 + p2 + 2 * p1 + p0 + q0 + q1, 3);
    *op0 = ROUND_POWER_OF_TWO(p3 + p2 + p1 + 2 * p0 + q0 + q1 + q2, 3);
    *oq0 = ROUND_POWER_OF_TWO(p2 + p1 + p0 + 2 * q0 + q1 + q2 + q3, 3);
    *oq1 = ROUND_POWER_OF_TWO(p1 + p0 + q0 + 2 * q1 + q2 + q3 + q3, 3);
    *oq2 = ROUND_POWER_OF_TWO(p0 + q0 + q1 + 2 * q2 + q3 + q3 + q3, 3);

  } else {
    filter4(mask, thresh, op1,  op0, oq0, oq1);
  }
}

void vp9_lpf_horizontal_8_c(__global uint8_t *s, int p,  __constant const uint8_t *blimit,
                            __constant const uint8_t *limit,  __constant const uint8_t *thresh,
                            int count) {
  int i;

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < 8 * count; ++i) {
    const uint8_t p3 = s[-4 * p], p2 = s[-3 * p], p1 = s[-2 * p], p0 = s[-p];
    const uint8_t q0 = s[0 * p], q1 = s[1 * p], q2 = s[2 * p], q3 = s[3 * p];

    const int8_t mask = filter_mask(*limit, *blimit,
                                    p3, p2, p1, p0, q0, q1, q2, q3);
    const int8_t flat = flat_mask4(1, p3, p2, p1, p0, q0, q1, q2, q3);
    filter8(mask, *thresh, flat, s - 4 * p, s - 3 * p, s - 2 * p, s - 1 * p,
                                 s,         s + 1 * p, s + 2 * p, s + 3 * p);
    ++s;
  }
}

void vp9_lpf_horizontal_8_dual_c(__global uint8_t *s, int p,  __constant const uint8_t *blimit0,
                                  __constant const uint8_t *limit0,  __constant const uint8_t *thresh0,
                                  __constant const uint8_t *blimit1,  __constant const uint8_t *limit1,
                                  __constant const uint8_t *thresh1) {
  vp9_lpf_horizontal_8_c(s, p, blimit0, limit0, thresh0, 1);
  vp9_lpf_horizontal_8_c(s + 8, p, blimit1, limit1, thresh1, 1);
}

void vp9_lpf_vertical_8_c(__global uchar4 *s, int pitch,  __constant const uint8_t *blimit,
                          __constant const uint8_t *limit,  __constant const uint8_t *thresh,
                          int count) {
  int i;

  for (i = 0; i < 8 * count; ++i) {
    const uchar4 sl = s[-1], sr = s[0];
   // const uint8_t p3 = s[-4], p2 = s[-3], p1 = s[-2], p0 = s[-1];
   // const uint8_t q0 = s[0], q1 = s[1], q2 = s[2], q3 = s[3];
    const int8_t mask = filter_mask(*limit, *blimit,
                                    sl.x, sl.y, sl.z, sl.w, sr.x, sr.y, sr.z, sr.w);
    const int8_t flat = flat_mask4(1, sl.x, sl.y, sl.z, sl.w, sr.x, sr.y, sr.z, sr.w);
    FILTER_8(mask, *thresh, flat, sl.x, sl.y ,sl.z, sl.w, sr.x, sr.y, sr.z, sr.w);
    s[-1] = sl;
    s[0] = sr;
    s += pitch;
  }
}

void vp9_lpf_vertical_8_dual_c(__global uchar4 *s, int pitch,  __constant const uint8_t *blimit0,
                                __constant const uint8_t *limit0,  __constant const uint8_t *thresh0,
                                __constant const uint8_t *blimit1,  __constant const uint8_t *limit1,
                                __constant const uint8_t *thresh1) {
  vp9_lpf_vertical_8_c(s, pitch, blimit0, limit0, thresh0, 1);
  vp9_lpf_vertical_8_c(s + 8 * pitch, pitch, blimit1, limit1,
                                    thresh1, 1);
}



void filter16(int8_t mask, uint8_t thresh,
                            uint8_t flat, uint8_t flat2,
                            __global uint8_t *op7, __global uint8_t *op6,
                            __global uint8_t *op5, __global uint8_t *op4,
                            __global uint8_t *op3, __global uint8_t *op2,
                            __global uint8_t *op1, __global uint8_t *op0,
                            __global uint8_t *oq0, __global uint8_t *oq1,
                            __global uint8_t *oq2, __global uint8_t *oq3,
                            __global uint8_t *oq4, __global uint8_t *oq5,
                            __global uint8_t *oq6, __global uint8_t *oq7) {
  if (flat2 && flat && mask) {
    const uint8_t p7 = *op7, p6 = *op6, p5 = *op5, p4 = *op4,
                  p3 = *op3, p2 = *op2, p1 = *op1, p0 = *op0;

    const uint8_t q0 = *oq0, q1 = *oq1, q2 = *oq2, q3 = *oq3,
                  q4 = *oq4, q5 = *oq5, q6 = *oq6, q7 = *oq7;

    // 15-tap filter [1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1]
    *op6 = ROUND_POWER_OF_TWO(p7 * 7 + p6 * 2 + p5 + p4 + p3 + p2 + p1 + p0 +
                              q0, 4);
    *op5 = ROUND_POWER_OF_TWO(p7 * 6 + p6 + p5 * 2 + p4 + p3 + p2 + p1 + p0 +
                              q0 + q1, 4);
    *op4 = ROUND_POWER_OF_TWO(p7 * 5 + p6 + p5 + p4 * 2 + p3 + p2 + p1 + p0 +
                              q0 + q1 + q2, 4);
    *op3 = ROUND_POWER_OF_TWO(p7 * 4 + p6 + p5 + p4 + p3 * 2 + p2 + p1 + p0 +
                              q0 + q1 + q2 + q3, 4);
    *op2 = ROUND_POWER_OF_TWO(p7 * 3 + p6 + p5 + p4 + p3 + p2 * 2 + p1 + p0 +
                              q0 + q1 + q2 + q3 + q4, 4);
    *op1 = ROUND_POWER_OF_TWO(p7 * 2 + p6 + p5 + p4 + p3 + p2 + p1 * 2 + p0 +
                              q0 + q1 + q2 + q3 + q4 + q5, 4);
    *op0 = ROUND_POWER_OF_TWO(p7 + p6 + p5 + p4 + p3 + p2 + p1 + p0 * 2 +
                              q0 + q1 + q2 + q3 + q4 + q5 + q6, 4);
    *oq0 = ROUND_POWER_OF_TWO(p6 + p5 + p4 + p3 + p2 + p1 + p0 +
                              q0 * 2 + q1 + q2 + q3 + q4 + q5 + q6 + q7, 4);
    *oq1 = ROUND_POWER_OF_TWO(p5 + p4 + p3 + p2 + p1 + p0 +
                              q0 + q1 * 2 + q2 + q3 + q4 + q5 + q6 + q7 * 2, 4);
    *oq2 = ROUND_POWER_OF_TWO(p4 + p3 + p2 + p1 + p0 +
                              q0 + q1 + q2 * 2 + q3 + q4 + q5 + q6 + q7 * 3, 4);
    *oq3 = ROUND_POWER_OF_TWO(p3 + p2 + p1 + p0 +
                              q0 + q1 + q2 + q3 * 2 + q4 + q5 + q6 + q7 * 4, 4);
    *oq4 = ROUND_POWER_OF_TWO(p2 + p1 + p0 +
                              q0 + q1 + q2 + q3 + q4 * 2 + q5 + q6 + q7 * 5, 4);
    *oq5 = ROUND_POWER_OF_TWO(p1 + p0 +
                              q0 + q1 + q2 + q3 + q4 + q5 * 2 + q6 + q7 * 6, 4);
    *oq6 = ROUND_POWER_OF_TWO(p0 +
                              q0 + q1 + q2 + q3 + q4 + q5 + q6 * 2 + q7 * 7, 4);
  } else {
    filter8(mask, thresh, flat, op3, op2, op1, op0, oq0, oq1, oq2, oq3);
  }
}

void vp9_lpf_horizontal_16_c(__global uint8_t *s, int p,  __constant const uint8_t *blimit,
                              __constant const uint8_t *limit,  __constant const uint8_t *thresh,
                             int count) {
  int i;

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < 8 * count; ++i) {
    const uint8_t p3 = s[-4 * p], p2 = s[-3 * p], p1 = s[-2 * p], p0 = s[-p];
    const uint8_t q0 = s[0 * p], q1 = s[1 * p], q2 = s[2 * p], q3 = s[3 * p];
    const int8_t mask = filter_mask(*limit, *blimit,
                                    p3, p2, p1, p0, q0, q1, q2, q3);
    const int8_t flat = flat_mask4(1, p3, p2, p1, p0, q0, q1, q2, q3);
    const int8_t flat2 = flat_mask5(1,
                             s[-8 * p], s[-7 * p], s[-6 * p], s[-5 * p], p0,
                             q0, s[4 * p], s[5 * p], s[6 * p], s[7 * p]);

    filter16(mask, *thresh, flat, flat2,
             s - 8 * p, s - 7 * p, s - 6 * p, s - 5 * p,
             s - 4 * p, s - 3 * p, s - 2 * p, s - 1 * p,
             s,         s + 1 * p, s + 2 * p, s + 3 * p,
             s + 4 * p, s + 5 * p, s + 6 * p, s + 7 * p);
    ++s;
  }
}

 void mb_lpf_vertical_edge_w(__global uchar4 *s, int p,
                                   __constant const uint8_t *blimit,
                                    __constant const uint8_t *limit,
                                    __constant const uint8_t *thresh,
                                   int count) {
  int i;

  for (i = 0; i < count; ++i) {
    uchar4 sl1 = s[-2], sl0 = s[-1], sr0 = s[0], sr1 = s[1];
   // const uint8_t p3 = sl0.x, p2 = sl0.y, p1 = sl0.z, p0 = sl0.w;
    //const uint8_t q0 = sr0.x, q1 = sr0.y,  q2 = sr0.z, q3 = sr0.w;
    //const int8_t mask = filter_mask(*limit, *blimit,
                                   // p3, p2, p1, p0, q0, q1, q2, q3);
     const int8_t mask = filter_mask(*limit, *blimit,
                                    sl0.x, sl0.y, sl0.z, sl0.w, sr0.x, sr0.y, sr0.z, sr0.w);
    const int8_t flat = flat_mask4(1, sl0.x, sl0.y, sl0.z, sl0.w, sr0.x, sr0.y, sr0.z, sr0.w);
    const int8_t flat2 = flat_mask5(1, sl1.x, sl1.y, sl1.z, sl1.w, sl0.w,
                                    sr0.x, sr1.x, sr1.y, sr1.z, sr1.w);

    FILTER_16(mask, *thresh, flat, flat2,
             sl1.x ,sl1.y,sl1.z,sl1.w, sl0.x ,sl0.y, sl0.z, sl0.w,
            sr0.x, sr0.y ,sr0.z, sr0.w, sr1.x, sr1.y, sr1.z, sr1.w);
   s[-2] = sl1;
   s[-1] = sl0;
   s[0] = sr0;
   s[1] = sr1;
    s += p;
  }
}

void vp9_lpf_vertical_16_c(__global uchar4 *s, int p,  __constant const uint8_t *blimit,
                            __constant const uint8_t *limit,  __constant const uint8_t *thresh) {
  mb_lpf_vertical_edge_w(s, p, blimit, limit, thresh, 8);
}

void vp9_lpf_vertical_16_dual_c(__global uchar4 *s, int p,  __constant const uint8_t *blimit,
                                 __constant const uint8_t *limit,  __constant const uint8_t *thresh) {
  mb_lpf_vertical_edge_w(s, p, blimit, limit, thresh, 16);
}

#if 1
void filter_selectively_vert_row2(
                                  __global uchar4 *s, 
                                  int pitch,
                                  uint mask_16x16_l,
                                  uint mask_8x8_l,
                                  uint mask_4x4_l,
                                  uint mask_4x4_int_l,
                                  __constant const loop_filter_info_n_ocl* lfi_n,
                                  __global const uchar *lfl
) {

 // const int mask_shift = 8;//plane_type ? 4 : 8;
 // const int mask_cutoff = 0xff;//plane_type ? 0xf : 0xff;
 // const int lfl_forward = 8;//plane_type ? 4 : 8;

  uint mask_16x16_0 = mask_16x16_l & 0xff;
  uint mask_8x8_0 = mask_8x8_l & 0xff;
  uint mask_4x4_0 = mask_4x4_l & 0xff;
  uint mask_4x4_int_0 = mask_4x4_int_l & 0xff;
  uint mask_16x16_1 = (mask_16x16_l >> 8) & 0xff;
  uint mask_8x8_1 = (mask_8x8_l >> 8) & 0xff;
  uint mask_4x4_1 = (mask_4x4_l >> 8) & 0xff;
  uint mask_4x4_int_1 = (mask_4x4_int_l >> 8) & 0xff;
  uint mask;

  for (mask = mask_16x16_0 | mask_8x8_0 | mask_4x4_0 | mask_4x4_int_0 |
      mask_16x16_1 | mask_8x8_1 | mask_4x4_1 | mask_4x4_int_1;
      mask; mask >>= 1) {

     __constant const loop_filter_thresh_ocl *lfi0 = (lfi_n[0].lfthr + (*lfl));
  
     __constant const loop_filter_thresh_ocl *lfi1 = (lfi_n[0].lfthr + *(lfl + /*lfl_forward*/8));
     

    // TODO(yunqingwang): count in loopfilter functions should be removed.
    if (mask & 1) {

      if ((mask_16x16_0 | mask_16x16_1) & 1) {
        if ((mask_16x16_0 & mask_16x16_1) & 1) {
          vp9_lpf_vertical_16_dual_c(s, pitch, lfi0[0].mblim, lfi0[0].lim,
                                   lfi0[0].hev_thr);
        } else if (mask_16x16_0 & 1) {
          vp9_lpf_vertical_16_c(s, pitch, lfi0[0].mblim, lfi0[0].lim,
                              lfi0[0].hev_thr);
        } else {
          vp9_lpf_vertical_16_c(s + 8 *pitch, pitch, lfi1[0].mblim,
                              lfi1[0].lim, lfi1[0].hev_thr);
        }
      }

      if ((mask_8x8_0 | mask_8x8_1) & 1) {
        if ((mask_8x8_0 & mask_8x8_1) & 1) {
          vp9_lpf_vertical_8_dual_c(s, pitch, lfi0[0].mblim, lfi0[0].lim,
                                  lfi0[0].hev_thr, lfi1[0].mblim, lfi1[0].lim,
                                  lfi1[0].hev_thr);
        } else if (mask_8x8_0 & 1) {
          vp9_lpf_vertical_8_c(s, pitch, lfi0[0].mblim, lfi0[0].lim, lfi0[0].hev_thr,
                             1);
        } else {
          vp9_lpf_vertical_8_c(s + 8 * pitch, pitch, lfi1[0].mblim, lfi1[0].lim,
                             lfi1[0].hev_thr, 1);
        }
      }

      if ((mask_4x4_0 | mask_4x4_1) & 1) {
        if ((mask_4x4_0 & mask_4x4_1) & 1) {
          vp9_lpf_vertical_4_dual_c(s, pitch, lfi0[0].mblim, lfi0[0].lim,
                                  lfi0[0].hev_thr, lfi1[0].mblim, lfi1[0].lim,
                                  lfi1[0].hev_thr);
        } else if (mask_4x4_0 & 1) {
          vp9_lpf_vertical_4_c(s, pitch, lfi0[0].mblim, lfi0[0].lim, lfi0[0].hev_thr,
                             1);
        } else {
          vp9_lpf_vertical_4_c(s + 8 * pitch, pitch, lfi1[0].mblim, lfi1[0].lim,
                             lfi1[0].hev_thr, 1);
        }
      }

      if ((mask_4x4_int_0 | mask_4x4_int_1) & 1) {
        if ((mask_4x4_int_0 & mask_4x4_int_1) & 1) {
          vp9_lpf_vertical_4_dual_c(s + 1, pitch, lfi0[0].mblim, lfi0[0].lim,
                                  lfi0[0].hev_thr, lfi1[0].mblim, lfi1[0].lim,
                                  lfi1[0].hev_thr);
        } else if (mask_4x4_int_0 & 1) {
          vp9_lpf_vertical_4_c(s + 1, pitch, lfi0[0].mblim, lfi0[0].lim,
                             lfi0[0].hev_thr, 1);
        } else {
          vp9_lpf_vertical_4_c(s + 8 * pitch + 1, pitch, lfi1[0].mblim, lfi1[0].lim,
                             lfi1[0].hev_thr, 1);
        }
      }
    }

    s += (8 >> 2);
    lfl += 1;
    mask_16x16_0 >>= 1;
    mask_8x8_0 >>= 1;
    mask_4x4_0 >>= 1;
    mask_4x4_int_0 >>= 1;
    mask_16x16_1 >>= 1;
    mask_8x8_1 >>= 1;
    mask_4x4_1 >>= 1;
    mask_4x4_int_1 >>= 1;

  }

}
#endif


void filter_selectively_horiz(__global uchar *s, 
                                     int pitch,
                                     uint mask_16x16,
                                     uint mask_8x8,
                                     uint mask_4x4,
                                     uint mask_4x4_int,
                                     __constant const loop_filter_info_n_ocl *lfi_n,
                                     __global const uchar *lfl
                                    ) {

  uint mask;
  int count = 1;
  
 
  for (mask = mask_16x16 | mask_8x8 | mask_4x4| mask_4x4_int;
       mask; mask >>= count) {
    __constant const loop_filter_thresh_ocl *lfi = (lfi_n[0].lfthr + (*lfl));
  

    count = 1;
    if (mask & 1) {
      if (mask_16x16 & 1) {
        if ((mask_16x16 & 3) == 3) {
          vp9_lpf_horizontal_16_c(s, pitch, lfi[0].mblim, lfi[0].lim,
                                lfi[0].hev_thr, 2);
          count = 2;
        } else {
          vp9_lpf_horizontal_16_c(s, pitch, lfi[0].mblim, lfi[0].lim,
                                lfi[0].hev_thr, 1);
        }
      } else if (mask_8x8 & 1) {
        if ((mask_8x8 & 3) == 3) {
          // Next block's thresholds
          __constant const loop_filter_thresh_ocl *lfin = (lfi_n[0].lfthr + *(lfl + 1));

          vp9_lpf_horizontal_8_dual_c(s, pitch, lfi[0].mblim, lfi[0].lim,
                                    lfi[0].hev_thr, lfin[0].mblim, lfin[0].lim,
                                    lfin[0].hev_thr);
         

          if ((mask_4x4_int & 3) == 3) {
            vp9_lpf_horizontal_4_dual_c(s + 4 * pitch, pitch, lfi[0].mblim,
                                      lfi[0].lim, lfi[0].hev_thr, lfin[0].mblim,
                                      lfin[0].lim, lfin[0].hev_thr);
          } else {
            if (mask_4x4_int & 1)
      
              vp9_lpf_horizontal_4_c(s + 4 * pitch, pitch, lfi[0].mblim, lfi[0].lim,
                                   lfi[0].hev_thr, 1);
            else if (mask_4x4_int & 2)
        
              vp9_lpf_horizontal_4_c(s + 8 + 4 * pitch, pitch, lfin[0].mblim,
                                   lfin[0].lim, lfin[0].hev_thr, 1);
          }
          count = 2;
        } else {
          vp9_lpf_horizontal_8_c(s, pitch, lfi[0].mblim, lfi[0].lim, lfi[0].hev_thr, 1);

          if (mask_4x4_int & 1)
            vp9_lpf_horizontal_4_c(s + 4 * pitch, pitch, lfi[0].mblim, lfi[0].lim,
                                 lfi[0].hev_thr, 1);
        }
      }else if (mask_4x4 & 1) {
        if ((mask_4x4 & 3) == 3) {
          // Next block's thresholds
          __constant const loop_filter_thresh_ocl *lfin = (lfi_n[0].lfthr + *(lfl + 1));

          vp9_lpf_horizontal_4_dual_c(s, pitch, lfi[0].mblim, lfi[0].lim,
                                   lfi[0].hev_thr, lfin[0].mblim, lfin[0].lim,
                                   lfin[0].hev_thr);
          if ((mask_4x4_int & 3) == 3) {
            vp9_lpf_horizontal_4_dual_c(s + 4 * pitch, pitch, lfi[0].mblim,
                                      lfi[0].lim, lfi[0].hev_thr, lfin[0].mblim,
                                      lfin[0].lim, lfin[0].hev_thr);
          } else {
            if (mask_4x4_int & 1)
              vp9_lpf_horizontal_4_c(s + 4 * pitch, pitch, lfi[0].mblim, lfi[0].lim,
                                   lfi[0].hev_thr, 1);
            else if (mask_4x4_int & 2)
              vp9_lpf_horizontal_4_c(s + 8 + 4 * pitch, pitch, lfin[0].mblim,
                                   lfin[0].lim, lfin[0].hev_thr, 1);
          }
          count = 2;
        } else {
          vp9_lpf_horizontal_4_c(s, pitch, lfi[0].mblim, lfi[0].lim, lfi[0].hev_thr, 1);

          if (mask_4x4_int & 1)
            vp9_lpf_horizontal_4_c(s + 4 * pitch, pitch, lfi[0].mblim, lfi[0].lim,
                                 lfi[0].hev_thr, 1);
        }
      } else if (mask_4x4_int & 1) {
        vp9_lpf_horizontal_4_c(s + 4 * pitch, pitch, lfi[0].mblim, lfi[0].lim,
                             lfi[0].hev_thr, 1);
      }
    }

    s += 8 * count;
    lfl += count;
    mask_16x16 >>= count;
    mask_8x8 >>= count;
    mask_4x4 >>= count;
    mask_4x4_int >>= count;

  }

}

/**********************************************************************************************/
__kernel void filter_block_plane(__global uchar *new_buffer,
                                 __global  LOOPFILTER_PARAM_OCL* loopfilter_param_ocl,
                                 __global  LOOP_FILTER_MASK_OCL* lfm,
                                 __constant loop_filter_info_n_ocl* lf_info_n_ocl,
                                int block_count) {
  int block_id = get_global_id(0);
  if(block_id >= block_count)
    return;
 
  int mi_row = loopfilter_param_ocl[block_id].mi_row;
  
  int mi_rows = loopfilter_param_ocl[block_id].mi_rows;
  int dst_stride = loopfilter_param_ocl[block_id].dst_stride;
  __global uchar *dst0 = new_buffer + loopfilter_param_ocl[block_id].dst_offset;
  __global uchar *dst_tmpbuf = dst0;
 
  
    uint32_t mask_16x16 = lfm[block_id].left_y[2];
    
    uint32_t mask_8x8 = lfm[block_id].left_y[1];
  
    uint32_t mask_4x4 = lfm[block_id].left_y[0];
 
    uint32_t mask_4x4_int = lfm[block_id].int_4x4_y;
    

  // Vertical pass: do 2 rows at one time
    int r, c;
   //
    for(r = 0; r < MI_BLOCK_SIZE && mi_row + r < mi_rows; r += 2 ) {
     // uint mask_16x16_l = mask_16x16 & 0xffff;
      //uint mask_8x8_l = mask_8x8 & 0xffff;
      //uint mask_4x4_l = mask_4x4 & 0xffff;
      //uint mask_4x4_int_l = mask_4x4_int & 0xffff;
    
      // Disable filtering on the leftmost column
      filter_selectively_vert_row2(
                                   (__global uchar4 *)dst_tmpbuf, (dst_stride >> 2),
                                   mask_16x16 & 0xffff,
                                   mask_8x8 & 0xffff,
                                   mask_4x4 & 0xffff,
                                   mask_4x4_int & 0xffff,
                                   lf_info_n_ocl,
                                   lfm[block_id].lfl_y + (r << 3));

      dst_tmpbuf += 16 * dst_stride;
      mask_16x16 >>= 16;
      mask_8x8 >>= 16;
      mask_4x4 >>= 16;
      mask_4x4_int >>= 16;
      
    }

  //horizon pass
    dst_tmpbuf = dst0;
    mask_16x16 = lfm[block_id].above_y[2];
    mask_8x8 = lfm[block_id].above_y[1];
    mask_4x4 = lfm[block_id].above_y[0];
    mask_4x4_int = lfm[block_id].int_4x4_y;

    for (r = 0; r < MI_BLOCK_SIZE && mi_row + r < mi_rows; r++) {
      unsigned int mask_16x16_r;
      unsigned int mask_8x8_r;
      unsigned int mask_4x4_r;
     

      if (mi_row + r == 0) {
        mask_16x16_r = 0;
        mask_8x8_r = 0;
        mask_4x4_r = 0;
      } else {
        mask_16x16_r = mask_16x16 & 0xff;
        mask_8x8_r = mask_8x8 & 0xff;
        mask_4x4_r = mask_4x4 & 0xff;
      }
     

      filter_selectively_horiz(dst_tmpbuf, dst_stride,
                               mask_16x16_r,
                               mask_8x8_r,
                               mask_4x4_r,
                               mask_4x4_int & 0xff,
                               lf_info_n_ocl,
                               lfm[block_id].lfl_y  + (r << 3)
                               );

      dst_tmpbuf += 8 * dst_stride;
      mask_16x16 >>= 8;
      mask_8x8 >>= 8;
      mask_4x4 >>= 8;
      mask_4x4_int >>= 8;
    }

}
