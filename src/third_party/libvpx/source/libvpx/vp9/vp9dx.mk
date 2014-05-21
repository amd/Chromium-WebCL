##
##  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
##
##  Use of this source code is governed by a BSD-style license
##  that can be found in the LICENSE file in the root of the source
##  tree. An additional intellectual property rights grant can be found
##  in the file PATENTS.  All contributing project authors may
##  be found in the AUTHORS file in the root of the source tree.
##

VP9_DX_EXPORTS += exports_dec

VP9_DX_SRCS-yes += $(VP9_COMMON_SRCS-yes)
VP9_DX_SRCS-no  += $(VP9_COMMON_SRCS-no)
VP9_DX_SRCS_REMOVE-yes += $(VP9_COMMON_SRCS_REMOVE-yes)
VP9_DX_SRCS_REMOVE-no  += $(VP9_COMMON_SRCS_REMOVE-no)

VP9_DX_SRCS-yes += vp9_dx_iface.c

VP9_DX_SRCS-yes += decoder/vp9_decodemv.c

VP9_DX_SRCS-yes += decoder/vp9_detokenize.c
VP9_DX_SRCS-yes += decoder/vp9_reader.h
VP9_DX_SRCS-yes += decoder/vp9_reader.c
VP9_DX_SRCS-yes += decoder/vp9_read_bit_buffer.h
VP9_DX_SRCS-yes += decoder/vp9_decodemv.h
VP9_DX_SRCS-yes += decoder/vp9_detokenize.h
VP9_DX_SRCS-yes += decoder/vp9_onyxd.h
VP9_DX_SRCS-yes += decoder/vp9_onyxd_int.h
VP9_DX_SRCS-yes += decoder/vp9_thread.c
VP9_DX_SRCS-yes += decoder/vp9_thread.h
VP9_DX_SRCS-yes += decoder/vp9_onyxd_if.c
VP9_DX_SRCS-yes += decoder/vp9_dsubexp.c
VP9_DX_SRCS-yes += decoder/vp9_dsubexp.h
VP9_DX_SRCS-yes += decoder/vp9_decodeframe_recon.c
VP9_DX_SRCS-yes += decoder/vp9_decodeframe_recon.h
VP9_DX_SRCS-yes += decoder/vp9_append.c
VP9_DX_SRCS-yes += decoder/vp9_append.h

#VP9_DX_SRCS-yes += decoder/vp9_copy_mip_ocl.c
#VP9_DX_SRCS-yes += decoder/vp9_copy_mip_ocl.h

VP9_DX_SRCS-yes += decoder/vp9_intra_predict.c
VP9_DX_SRCS-yes += decoder/vp9_intra_predict.h
VP9_DX_SRCS-yes += decoder/vp9_detokenize_recon.c
VP9_DX_SRCS-yes += decoder/vp9_detokenize_recon.h

P9_DX_SRCS-yes += sched/list.h
VP9_DX_SRCS-yes += sched/thread.h
VP9_DX_SRCS-yes += sched/queue.h
VP9_DX_SRCS-yes += sched/queue.c
VP9_DX_SRCS-yes += sched/task.h
VP9_DX_SRCS-yes += sched/task.c
VP9_DX_SRCS-yes += sched/device.h
VP9_DX_SRCS-yes += sched/device.c
VP9_DX_SRCS-yes += sched/sched.h
VP9_DX_SRCS-yes += sched/sched.c
VP9_DX_SRCS-yes += sched/step.h
VP9_DX_SRCS-yes += sched/step.c
VP9_DX_SRCS-yes += sched/atomic.h

VP9_DX_SRCS-yes += decoder/vp9_device.h
VP9_DX_SRCS-yes += decoder/vp9_device.c
VP9_DX_SRCS-yes += decoder/vp9_step.h
VP9_DX_SRCS-yes += decoder/vp9_step.c
VP9_DX_SRCS-yes += decoder/vp9_entropy_step.h
VP9_DX_SRCS-yes += decoder/vp9_entropy_step.c

VP9_DX_SRCS-yes += decoder/vp9_loopfilter_recon.c
VP9_DX_SRCS-yes += decoder/vp9_loopfilter_recon.h
VP9_DX_SRCS-yes += decoder/vp9_loopfilter_step.c
VP9_DX_SRCS-yes += decoder/vp9_loopfilter_step.h

VP9_DX_SRCS-yes += decoder/vp9_tile_info.h

# PPA
VP9_DX_SRCS-yes += ppa.h

VP9_DX_SRCS-yes += ppa.c

VP9_DX_SRCS-yes += ppaCPUEvents.h

VP9_DX_SRCS-yes := $(filter-out $(VP9_DX_SRCS_REMOVE-yes),$(VP9_DX_SRCS-yes))
