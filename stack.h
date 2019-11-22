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

#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

#define alloc malloc

/*  SECTION: Types  */
typedef struct stack Stack;
typedef enum stackOption StackOption;

enum stackOption {
  STACK = 0,
  STACKCONCURRENT,
};

struct stack {
  struct node** head; 
  void (*nd)(Data);
#if (defined(_THREAD_H_))
  Mutex lock;
#endif
  uintmax_t options, size, datatype;
};

/*  SECTION: Prototypes */
Stack*
NewConcurrentStack(void);

Stack*
NewStack(void);

bool
StackPush(Stack* const, Data);

bool
StackPushAt(Stack* const, int, Data);

Node*
StackPop(Stack* const);

Data
StackPopValue(Stack* const);

Node*
StackPopAt(Stack* const, int);

Data
StackPopValueAt(Stack* const, int);

Node*
StackItemAt(Stack* const, int);

bool
StackRemoveAt(Stack* const, int);

bool
StackReverse(Stack* const);

bool
StackContains(Stack* const, Data);

Node**
StackAsArray(Stack* const);

bool
StackIsEmpty(Stack* const);

void
RemoveStack(Stack*);

void
ResetStack(Stack* const);

// getters

uintmax_t
StackDataType(Stack*);

// setters

void
StackSetNodeDeallocator(Stack* const, void (*nd)(Data));

void
StackSetDataType(Stack* const, uintmax_t);

#endif
