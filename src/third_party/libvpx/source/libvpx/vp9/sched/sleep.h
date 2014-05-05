/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SCHED_SLEEP_H_
#define SCHED_SLEEP_H_

#if defined(_WIN32)
#include <windows.h>
static INLINE void msleep(int msec) {
  Sleep(msec);
}
#else
#include <unistd.h>
static INLINE void msleep(int msec) {
  usleep(msec * 1000);
}
#endif

#endif  // SCHED_SLEEP_H_
