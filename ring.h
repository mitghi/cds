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

#ifndef __RING_H__
#define __RING_H__

#include <stdbool.h>
#include "types.h"

#define alloc malloc

/* SECTION: Types */
typedef struct ring Ring;
typedef enum ringOption RingOption;

enum ringOption {
    RING = 0,
    RINGCONCURRENT,
};

/* SECTION: Structs */
struct ring {
#if (defined(_THREAD_H_))
  Mutex		 lock;
#endif
  uintptr_t*    elements; 	
  void (*nd)(Data);
  uint64_t	SI;
  uint64_t	LI;
  uint64_t	Mask;
  uintmax_t	options, size, datatype;
};

/* SECTION: Prototypes */
Ring*
NewRing(uint64_t);

Ring*
NewConcurrentRing(uint64_t);

void
RingPush(Ring*, uintptr_t);

Data
RingPop(Ring*);

void
RemoveRing(Ring*);

void
RingSetNodeDeallocator(Ring*, void (*nd)(Data));

void
RingSetDataType(Ring*, uintmax_t);

#endif
