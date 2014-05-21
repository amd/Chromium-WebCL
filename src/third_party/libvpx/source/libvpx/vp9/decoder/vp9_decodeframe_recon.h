/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#ifndef VP9_DECODER_VP9_DECODEFRAME_RECON_H_
#define VP9_DECODER_VP9_DECODEFRAME_RECON_H_

struct VP9Common;
struct VP9Decompressor;

int vp9_decode_frame_recon(struct VP9Decompressor *cpi,
                           const uint8_t **p_data_end);


void decode_tile_recon_entropy(VP9D_COMP *pbi, const TileInfo *const tile,
                               vp9_reader *r, int tile_col);

void decode_tile_recon_entropy_for_entropy(VP9D_COMP *pbi,
                               const TileInfo *const tile,
                               vp9_reader *r, int tile_col);

void decode_tile_recon_inter_transform(VP9D_COMP *pbi,
                                       const TileInfo *const tile,
                                       vp9_reader *r, int tile_col);

void decode_tile_recon_inter(VP9D_COMP *pbi, const TileInfo *const tile,
                             vp9_reader *r, int tile_col);

void decode_tile_recon_inter_ocl(VP9D_COMP *pbi, const TileInfo *const tile,
                                 vp9_reader *r, int tile_col);

void decode_tile_recon_intra(VP9D_COMP *pbi, const TileInfo *const tile,
                             vp9_reader *r, int tile_col);

int vp9_decode_frame_tail(VP9D_COMP *pbi);

int vp9_decode_frame_mt_entropy_recon(VP9D_COMP *pbi, VP9D_COMP **storage_pbi,
                                                    const uint8_t **p_data_end);


int vp9_decode_frame_mt_entropy_recon_last_frame(VP9D_COMP *pbi,
                                                                  VP9D_COMP **storage_pbi,
                                                                  const uint8_t **p_data_end);



void decode_tile_recon_inter_prepare_ocl(VP9D_COMP *pbi,
                                         const TileInfo *const tile,
                                         vp9_reader *r, int tile_col);

void decode_tile_recon_inter_index_ocl(VP9D_COMP *pbi,
                                       const TileInfo *const tile,
                                       vp9_reader *r, int tile_col);

void decode_tile_recon_inter_calcu_ocl(VP9D_COMP *pbi,
                                       const TileInfo *const tile,
                                       vp9_reader *r, int tile_col,
                                       int dev_gpu);

#endif  // VP9_DECODER_VP9_DECODEFRAME_RECON_H_
