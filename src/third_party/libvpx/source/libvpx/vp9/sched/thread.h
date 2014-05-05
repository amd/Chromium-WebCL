// Copyright 2013 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
// Multi-threaded worker
//
// Original source:
//  http://git.chromium.org/webm/libwebp.git
//  100644 blob eff8f2a8c20095aade3c292b0e9292dac6cb3587  src/utils/thread.c

#ifndef SCHED_THREAD_H_
#define SCHED_THREAD_H_

#include "vp9/decoder/vp9_thread.h"

#if defined(_WIN32)

//------------------------------------------------------------------------------
// simplistic pthread emulation layer

#include <process.h>

// _beginthreadex requires __stdcall
#define THREADFN unsigned int __stdcall
#define THREAD_RETURN(val) (unsigned int)((DWORD_PTR)val)

static INLINE int pthread_create(pthread_t* const thread, const void* attr,
                          unsigned int (__stdcall *start)(void*), void* arg) {
  (void)attr;
  *thread = (pthread_t)_beginthreadex(NULL,   /* void *security */
                                      0,      /* unsigned stack_size */
                                      start,
                                      arg,
                                      0,      /* unsigned initflag */
                                      NULL);  /* unsigned *thrdaddr */
  if (*thread == NULL) return 1;
  SetThreadPriority(*thread, THREAD_PRIORITY_ABOVE_NORMAL);
  return 0;
}

static INLINE int pthread_join(pthread_t thread, void** value_ptr) {
  (void)value_ptr;
  return (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0 ||
          CloseHandle(thread) == 0);
}

// Mutex
static INLINE int pthread_mutex_init(pthread_mutex_t* const mutex, void* mutexattr) {
  (void)mutexattr;
  InitializeCriticalSection(mutex);
  return 0;
}

static INLINE int pthread_mutex_lock(pthread_mutex_t* const mutex) {
  EnterCriticalSection(mutex);
  return 0;
}

static INLINE int pthread_mutex_unlock(pthread_mutex_t* const mutex) {
  LeaveCriticalSection(mutex);
  return 0;
}

static INLINE int pthread_mutex_destroy(pthread_mutex_t* const mutex) {
  DeleteCriticalSection(mutex);
  return 0;
}

// Condition
static INLINE int pthread_cond_destroy(pthread_cond_t* const condition) {
  int ok = 1;
  ok &= (CloseHandle(condition->waiting_sem_) != 0);
  ok &= (CloseHandle(condition->received_sem_) != 0);
  ok &= (CloseHandle(condition->signal_event_) != 0);
  return !ok;
}

static INLINE int pthread_cond_init(pthread_cond_t* const condition, void* cond_attr) {
  (void)cond_attr;
  condition->waiting_sem_ = CreateSemaphore(NULL, 0, 10, NULL);
  condition->received_sem_ = CreateSemaphore(NULL, 0, 10, NULL);
  condition->signal_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (condition->waiting_sem_ == NULL ||
      condition->received_sem_ == NULL ||
      condition->signal_event_ == NULL) {
    pthread_cond_destroy(condition);
    return 1;
  }
  return 0;
}

static INLINE int pthread_cond_signal(pthread_cond_t* const condition) {
  int ok = 1;
  if (WaitForSingleObject(condition->waiting_sem_, 0) == WAIT_OBJECT_0) {
    // a thread is waiting in pthread_cond_wait: allow it to be notified
    ok = SetEvent(condition->signal_event_);
    // wait until the event is consumed so the signaler cannot consume
    // the event via its own pthread_cond_wait.
    ok &= (WaitForSingleObject(condition->received_sem_, INFINITE) !=
           WAIT_OBJECT_0);
  }
  return !ok;
}

static INLINE int pthread_cond_wait(pthread_cond_t* const condition,
                             pthread_mutex_t* const mutex) {
  int ok;
  // note that there is a consumer available so the signal isn't dropped in
  // pthread_cond_signal
  if (!ReleaseSemaphore(condition->waiting_sem_, 1, NULL))
    return 1;
  // now unlock the mutex so pthread_cond_signal may be issued
  pthread_mutex_unlock(mutex);
  ok = (WaitForSingleObject(condition->signal_event_, INFINITE) ==
        WAIT_OBJECT_0);
  ok &= ReleaseSemaphore(condition->received_sem_, 1, NULL);
  pthread_mutex_lock(mutex);
  return !ok;
}

#else  // _WIN32
# define THREADFN void*
# define THREAD_RETURN(val) val
#include <pthread.h>
#endif

#endif  // SCHED_THREAD_H_
