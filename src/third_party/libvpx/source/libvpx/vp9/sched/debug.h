/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SCHED_DEBUG_H_
#define SCHED_DEBUG_H_

#include <stdio.h>

#define msg(...) do {  \
  fprintf(stderr, __VA_ARGS__);  \
  fprintf(stderr, " -----> File(%s), Func(%s), Line(%d)\n", __FILE__, __func__, __LINE__);  \
} while (0)

#endif  // SCHED_DEBUG_H_
