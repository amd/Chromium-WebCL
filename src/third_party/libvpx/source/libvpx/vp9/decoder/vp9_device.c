/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vp9/decoder/vp9_device.h"

#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"

static struct device devs[] = {
  {
    DEV_CPU,            // type
    3,                  // threads_count
    6,                  // max_queue_tasks
    DEV_STAT_ENABLED,   //
  },
  {
    DEV_CPU,             // type
    4,                   // threads_count
    6,                   // max_queue_tasks
    DEV_STAT_DISABLED,   //
  },
  {
    DEV_GPU,            // type
    1,                  // threads_count
    2,                  // max_queue_tasks
    DEV_STAT_ENABLED,   //
  },
  {
    DEV_DSP,            // type
    3,                  // threads_count
    6,                  // max_queue_tasks
    DEV_STAT_ENABLED,   //
  },
};

void vp9_register_devices(struct scheduler *sched) {
  // For now, we can chose CPU and GPU dev
#if USE_INTER_PREDICT_OCL
  int devices_count = 3;
#else
  int devices_count = 2;
#endif // USE_INTER_PREDICT_OCL

  scheduler_add_devices(sched, devs, devices_count);
}
