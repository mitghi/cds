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
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "queue.h"
#include "test.h"

#define Test_QueueDefaultLength 8

void PrintQueue(Queue* q)
{
  if(q == nil) {
    return;
  } else if (q->head == nil) {
    return;
  }
  Node** current;
  current = q->head;
  while(*current != nil) {
    printf("%d ", (int) (*current)->value);
    current = &((*current)->next);
  }
  printf("\n");
};

void initDefaultQueue(Queue** q)
{
  *q = NewQueue();
  if(*q == nil) {
    fprintf(stderr, "NewQueue() fail.\n");    
    abort();
  }
  for(uintmax_t i = 0; i < Test_QueueDefaultLength; i++) {
    QueuePush(*q, (uintptr_t)i);
  }
};

status TestQueue(uintmax_t size)
{
  Node* n;
  Queue* q = NewQueue();
  if(q == nil) {
    fprintf(stderr, "NewQueue() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    QueuePush(q, (uintptr_t)i);    
  }
  for(uintmax_t i = 0; i < size; i++) {    
    n = QueuePop(q);
    if(n != nil) {
      free(n);
    }
  }
  RemoveQueue(q);
  return OK;
};

status TestConcurrentQueue(uintmax_t size)
{
  Node* n;
  Queue* q = NewConcurrentQueue();
  if(q == nil) {
    fprintf(stderr, "NewConcurrentQueue() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    QueuePush(q, (uintmax_t)i);
  }
#ifdef DEBUG
  PrintQueue(q);
#endif
  for(uintmax_t i = 0; i < size; i++) {
    n = QueuePop(q);
    if(n != nil) {
      free(n);
    }    
  }
  RemoveQueue(q);
  return OK;
};

status TestQueueReverse(uintmax_t size)
{
  Queue* q = NewQueue();
  if(q == nil) {
    fprintf(stderr, "NewQueue() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    QueuePush(q, (uintptr_t)i);    
  }
#ifdef DEBUG
  PrintQueue(q);
#endif  
  QueueReverse(q);
  RemoveQueue(q);
  return OK;
};

int main()
{
  assert(TestQueue(Test_QueueDefaultLength)	      == OK);
  assert(TestQueueReverse(Test_QueueDefaultLength)    == OK);
  assert(TestConcurrentQueue(Test_QueueDefaultLength) == OK);
  return 0;
};
