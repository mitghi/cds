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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ring.h"

#define nil NULL
#define _cast
#define MAXBYTE 0xffffffffffffffff

static const uintptr_t none = (uintptr_t)0;

/* SECTION: Utilities */
static const inline uint32_t
pw2_64(uint64_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v;  
};

uintmax_t
setBit(uintmax_t num, uintmax_t index, bool state)
{
  return state ? num | (1 << (--index)):
    num ^ ((MAXBYTE^(1<<(--index)))^MAXBYTE);
};

uintmax_t
getBit(uintmax_t num, uintmax_t index)
{
  return num & (1 << index);
};

/* SECTION: Forward declerations */
static Ring*
newRing(uint64_t);

static void
ringPush(Ring*, uintptr_t);
    
static Data
ringPop(Ring*);

static void
removeRing(Ring*);

inline static void
ringSetNodeDeallocator(Ring*, void (*)(Data));

inline static void
ringSetDataType(Ring*, uintmax_t);

/* SECTION: Internals */
static Ring*
newRing(uint64_t cap)
{
  Ring *r = _cast (Ring*) alloc(sizeof(*r));
  if(r == nil) {
    return nil;
  }
  memset(r, 0x00, sizeof(*r));
  r->Mask = pw2_64(cap);
  r->elements = _cast (uintptr_t*) alloc(r->Mask * sizeof(uintptr_t*));
  if(r->elements == nil) {
    goto ERROR;
  }
  return r;
  
 ERROR:
  free(r);
  return nil;  
};

static void
ringPush(Ring* r, uintptr_t value)
{
  r->elements[r->SI % r->Mask] = value;
  r->SI += 1;
  r->size += 1;  
};

static Data
ringPop(Ring* r)
{
  Data value;
  uint64_t index = r->LI % (r->Mask);
  value = r->elements[index];
  r->elements[index] = none;
  r->LI++;
  if(value != none) r->size--;
  return value;
};

static void
removeRing(Ring* r)
{
  if(r->elements != nil) {
    if(r->nd) {
      for(uint64_t i = 0; i < r->Mask; i++) {
        r->nd(r->elements[i%r->Mask]);
      }
    }
    free(r->elements);
  }
  free(r);
};

inline static void
ringSetNodeDeallocator(Ring* r, void (*nd)(Data))
{
  r->nd = nd;
  return;  
};

inline static void
ringSetDataType(Ring* r, uintmax_t datatype)
{
  r->datatype = datatype;
  return;  
};

/* SECTION: API */
Ring*
NewRing(uint64_t cap)
{
  return newRing(cap);  
};

Ring*
NewConcurrentRing(uint64_t cap)
{
#ifdef _THREAD_H_
  Ring* r = newRing(cap);
  r->options = setBit(r->options, 1, true);
  MutexInit(&r->lock);
  return r;
#else
#pragma message "newConcurrentRing(...) is not implemented."
#endif
  return nil;  
};

inline void
RingPush(Ring* r, uintptr_t value)
{
#ifdef _THREAD_H_
  if(getBit(r->options, 1) == RINGCONCURRENT) {
    MutexLock(&r->lock);
    ringPush(r, value);    
    MutexUnlock(&r->lock);
    return;
  }
#endif  
  ringPush(r, value);
};

Data
RingPop(Ring* r)
{
  Data value;  
#ifdef _THREAD_H_
  if(getBit(r->options, 1) == RINGCONCURRENT) {
    MutexLock(&r->lock);
    value = ringPop(r);    
    MutexUnlock(&r->lock);
    return value;
  }
#endif  
  value = ringPop(r);
  return value;  
};

bool
RingIsEmpty(Ring* r)
{  
#ifdef _THREAD_H_
  if(getBit(r->options, 1) == RINGCONCURRENT) {
    MutexLock(&r->lock);
    bool retval = r->size;    
    MutexUnlock(&r->lock);
    return retval == 0;
  }
#endif
  return r->size == 0;  
};

void
RemoveRing(Ring* r)
{
  removeRing(r);  
};

void
RingSetNodeDeallocator(Ring* r, void (*nd)(Data))
{
  ringSetNodeDeallocator(r, nd);
};

void
RingSetDataType(Ring* r, uintmax_t datatype)
{
  ringSetDataType(r, datatype);  
};
