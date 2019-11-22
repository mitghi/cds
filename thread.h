/*
*The MIT License
*
*Copyright (c) 2018 Mike (Milad) Taghavi <mitghi[at]gmail/me.com>
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in
*all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*THE SOFTWARE.
*/

#ifndef __THREAD_H__
#define __THREAD_H__

#ifndef _THREAD_H_
#define _THREAD_H_
#include <pthread.h>
typedef pthread_mutex_t Mutex;
/* SECTION: Prototypes */
void MutexInit(Mutex*);
void MutexTryLock(Mutex*);
void MutexLock(Mutex*);
void MutexUnlock(Mutex*);
void MutexDestroy(Mutex*);

/* SECTION: Implementation */
void MutexInit(Mutex* mutex)
{
  pthread_mutex_init(mutex, PTHREAD_MUTEX_NORMAL);
};

void
MutexTryLock(Mutex* mutex)
{
  pthread_mutex_trylock(mutex);
};

void
MutexLock(Mutex* mutex)
{
  pthread_mutex_lock(mutex);
};

void
MutexUnlock(Mutex* mutex)
{
  pthread_mutex_unlock(mutex);
};

void
MutexDestroy(Mutex* mutex)
{
  pthread_mutex_destroy(mutex);
};
/* #else */
/* #undef _THREAD_H_ */
#endif

#endif
