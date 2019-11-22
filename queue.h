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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

#define alloc malloc

/*  SECTION: Types  */
typedef struct queue Queue;
typedef enum queueOption QueueOption;

enum queueOption {
  QUEUE = 0,
  QUEUECONCURRENT,
};

/*  SECTION: Structs  */

struct queue {
  Node **head;
  Node **tail;
  void (*nd)(Data);
#if (defined(_THREAD_H_))
  Mutex lock;
#endif
  uintmax_t options, size, datatype;
};

/* SECTION: Prototypes */
Queue*
NewConcurrentQueue(void);

Queue*
NewQueue(void);

bool
QueuePush(Queue* const, const Data);

Node*
QueuePop(Queue* const);

Data
QueuePopValue(Queue* const);

Node*
QueuePopAt(Queue* const, const int);

Data
QueuePopValueAt(Queue* const, const int);

bool
QueueRemoveAt(Queue* const, const int);

bool
QueueContains(Queue* const, Data const);

bool
QueueIsEmpty(Queue* const);

Node**
QueueAsArray(Queue* const);

uintmax_t
QueueDataType(Queue* const);

void
QueueReverse(Queue* const);

void
RemoveQueue(Queue*);

void
ResetQueue(Queue* const);

void
QueueSetNodeDeallocator(Queue* const, void (*nd)(Data));

void
QueueSetDataType(Queue* const, uintmax_t);

int
QueueSize(Queue* const);

#endif
