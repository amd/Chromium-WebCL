/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_DECODER_VP9_ONYXD_INT_H_
#define VP9_DECODER_VP9_ONYXD_INT_H_

#include "./vpx_config.h"

#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/decoder/vp9_onyxd.h"
#include "vp9/decoder/vp9_thread.h"
#include "vp9/decoder/vp9_reader.h"
#include "vp9/sched/sched.h"
#include "vp9/decoder/vp9_tile_info.h"

#define MAX_TILES 4

typedef struct inter_pre_recon {
  int qcoeff_flag;
  int skip_coeff;
  int16_t offset;
  int eobtotal;  //it choose whether to do inv-transformation in the inter block
  int less8x8;  //make a flag that less than 8x8 block size
  TX_TYPE tx_type;  //tx_type for inv-transformation
  TX_MODE tx_mode;  //the inv-transifomation mode of the macro block
  BLOCK_SIZE bsize;  //the block size of the macro block
  uint8_t *dst[MAX_MB_PLANE];  //the macroblock address and stride in every frames.
  int mi_row;  //the row number of the macro block
  int mi_col;  //the col number of the macro block
  MODE_INFO **mi_8x8;
  int mb_to_left_edge;  //left edge of the macro block
  int mb_to_right_edge;  //right edge of the macro block
  int mb_to_top_edge;  //top edge of the macro block
  int mb_to_bottom_edge;  //bottom edge of the macro block
  int up_available;
  int left_available;
  int lossless;
  unsigned char skip_coeff_org;
} INTER_PRE_RECON;

typedef struct intra_pre_recon {
  int qcoeff_flag;
  int skip_coeff;
  int16_t offset;
  int eobtotal;  //it choose whether to do inv-transformation in the inter block
  int less8x8;  //make a flag that less than 8x8 block size
  TX_TYPE tx_type;  //tx_type for inv-transformation
  BLOCK_SIZE bsize;  //the block size of the macro block
  uint8_t *dst[MAX_MB_PLANE];  //the macroblock address and stride in every frames.
  int mi_row;  //the row number of the macro block
  int mi_col;  //the col number of the macro block
  MODE_INFO **mi_8x8;
  int mb_to_left_edge;  //left edge of the macro block
  int mb_to_right_edge;  //right edge of the macro block
  int mb_to_top_edge;  //top edge of the macro block
  int mb_to_bottom_edge;  //bottom edge of the macro block
  int up_available;
  int left_available;
  int lossless;
} INTRA_PRE_RECON;

typedef struct dequant_recon {
  DECLARE_ALIGNED(16, int16_t, qcoeff[MAX_MB_PLANE][64 * 64]);
  DECLARE_ALIGNED(16, uint16_t, eobs[MAX_MB_PLANE][256]);
} DEQUANT_RECON;

typedef struct loop_filter_recon{
  unsigned char filter_level;
  unsigned char skip_coeff;
  unsigned char ref0;      //the reference frame
  unsigned char tx_size_Y, tx_size_UV;
  unsigned char bsize;    //the block size of the macro block
}LOOP_FILTER_RECON;


typedef struct VP9_decoder_recon{
  DECLARE_ALIGNED(16, MACROBLOCKD, mb);
  DECLARE_ALIGNED(16, unsigned char, token_cache[1024]);
  DECLARE_ALIGNED(16, int16_t,  qcoeff[MAX_MB_PLANE][64 * 64]);
  DECLARE_ALIGNED(16, int16_t,  dqcoeff[MAX_MB_PLANE][64 * 64]);
  DECLARE_ALIGNED(16, uint16_t, eobs[MAX_MB_PLANE][256]);
  VP9_COMMON *cm;
  vp9_reader r;

  INTER_PRE_RECON *inter_pre_recon;  /*The infomation of inter prediction block */
  INTRA_PRE_RECON *intra_pre_recon;  /*The infomation of intra prediction block */
  DEQUANT_RECON *dequant_recon;      /*The infomation of dequant progress*/
  int inter_blocks_count;            /*This is a count for statistics inter block numbers  */
  int intra_blocks_count;            /*This is a count for statistics intra block numbers  */
  int dequant_count;

  TileInfo tile;
} VP9_DECODER_RECON;


typedef struct VP9Decompressor {
  DECLARE_ALIGNED(16, MACROBLOCKD, mb);

  DECLARE_ALIGNED(16, VP9_COMMON, common);

  DECLARE_ALIGNED(16, VP9_DECODER_RECON, decoder_recon[MAX_TILES]);

  DECLARE_ALIGNED(16, int16_t,  qcoeff[MAX_MB_PLANE][64 * 64]);
  DECLARE_ALIGNED(16, int16_t,  dqcoeff[MAX_MB_PLANE][64 * 64]);
  DECLARE_ALIGNED(16, uint16_t, eobs[MAX_MB_PLANE][256]);

  VP9D_CONFIG oxcf;

  const uint8_t *source;
  size_t source_sz;

  int64_t last_time_stamp;
  int ready_for_new_data;

  int refresh_frame_flags;

  int decoded_key_frame;

  int initial_width;
  int initial_height;

  int do_loopfilter_inline;  // apply loopfilter to available rows immediately
  VP9Worker lf_worker;

  VP9Worker *tile_workers;
  int num_tile_workers;

  /* Each tile column has its own MODE_INFO stream. This array indexes them by
     tile column index. */
  MODE_INFO **mi_streams;

  ENTROPY_CONTEXT *above_context[MAX_MB_PLANE];
  PARTITION_CONTEXT *above_seg_context;

  DECLARE_ALIGNED(16, uint8_t, token_cache[1024]);

  struct scheduler *sched;
  struct task_steps_pool *steps_pool;
  struct task_steps_pool *lf_steps_pool;
  struct task_cache *tsk_cache;
  struct task_cache *lf_tsk_cache;
  vp9_reader *last_reader;
  TileBuffer tile_buffers[4][1 << 6];
} VP9D_COMP;

#endif  // VP9_DECODER_VP9_ONYXD_INT_H_
