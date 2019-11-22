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
#include "stack.h"
#include "test.h"

#define Test_StackDefaultLength 8

void PrintStack(Stack* l)
{
  if(l == nil) {
    return;
  } else if (l->head == nil) {
    return;
  }
  Node** current;
  current = l->head;
  while(*current != nil) {
    printf("%d ", (int) (*current)->value);
    current = &((*current)->next);
  }
  printf("\n");
};

void initDefaultStack(Stack** l)
{
  *l = NewStack();
  if(*l == nil) {
    fprintf(stderr, "NewStack() fail.\n");    
    abort();
  }
  for(uintmax_t i = 0; i < Test_StackDefaultLength; i++) {
    StackPush(*l, (uintptr_t)i);    
  }
};

status TestStackPushAt(void)
{
  Stack* l = nil;
  initDefaultStack(&l);
  status isInserted = StackPushAt(l, 2, 100) == true ? OK: FAIL;
#ifdef DEBUG
  PrintStack(l);
#endif
  RemoveStack(l);
  return isInserted;
};

status TestStackPopAt(void)
{
  Stack* l = nil;
  initDefaultStack(&l);
  const int n = 5;
  int table[][n] = {
    {0, 7},
    {1, 5},
    {0, 0},
    {-1, 4},
    {-0, 1}
  };
  for(uintmax_t i = 0; i < n; i++) {
    int index = table[i][0];
    int expect = table[i][1];
    Node* n = StackPopAt(l, index);
    if(n == nil){      
      free(n);
      return FAIL;
    }
#ifdef DEBUG    
    printf("I: %d E: %d G: %d\n", index, expect, (int)n->value);
#endif    
    assert((int)n->value == expect);
    free(n);
    if(i == 1) {
      StackReverse(l);
    }
  }
#ifdef DEBUG
  PrintStack(l); /* 2 3 6 */
#endif
  RemoveStack(l);	
  return OK;
};

status TestStackReverse(uintmax_t size)
{
  Stack* l = NewStack();
  if(l == nil) {
    fprintf(stderr, "NewStack() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    StackPush(l, (uintptr_t)i);    
  }
  StackReverse(l);
#ifdef DEBUG
  PrintStack(l); /*  0 1 2 3 4 5 6 7 */
#endif
  RemoveStack(l);
  return OK;
};

status TestStack(uintmax_t size)
{
  Node* n;
  Stack* l = NewStack();
  if(l == nil) {
    fprintf(stderr, "NewStack() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    StackPush(l, (uintptr_t)i);    
  }
  for(uintmax_t i = 0; i < size; i++) {    
    n = StackPop(l);
    if(n != nil) {
      /* printf("value(%d)\n", (int)n->value); */
      free(n);
    }
  }
  RemoveStack(l);
  return OK;
};

status TestConcurrentStack(uintmax_t size)
{
  Node* n;
  Stack* l = NewConcurrentStack();
  if(l == nil) {
    fprintf(stderr, "NewConcurrentStack() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    StackPush(l, (uintptr_t)i);
  }
#ifdef DEBUG
  PrintStack(l);
#endif
  for(uintmax_t i = 0; i < size; i++) {
    n = StackPop(l);
    if(n != nil) {
      free(n);
    }
  }
  RemoveStack(l);
  return OK;
};

status TestStackItemAt(uintmax_t size)
{
  Node* n;
  Stack* l = NewStack();
  if(l == nil) {
    fprintf(stderr, "NewStack() fail.\n");
    abort();
    return FAIL;
  }
  for(uintmax_t i = 0; i < size; i++) {
    StackPush(l, (uintptr_t)i);    
  }
  n = StackItemAt(l, 1);
#ifdef DEBUG
  printf("value(n=%lu)\n", n->value);
#endif
  RemoveStack(l);
  return OK;
};

status TestStackAsArray(uintmax_t size)
{
  Node** n;
  Stack* s = nil;  
  initDefaultStack(&s);
  n = StackAsArray(s);
  if(n == nil) {
    RemoveStack(s);
    return FAIL;
  }
#ifdef DEBUG
  for(int i = 0; i < s->size; i++) {
    printf("n[%d] value(n=%lu)\n", i, n[i]->value);
  }
#endif
  free(n);
  RemoveStack(s);
  return OK;
};

int main()
{
  assert(TestStack(Test_StackDefaultLength)	      == OK);
  assert(TestConcurrentStack(Test_StackDefaultLength) == OK);  
  assert(TestStackReverse(Test_StackDefaultLength)    == OK);
  assert(TestStackItemAt(Test_StackDefaultLength)     == OK);
  assert(TestStackAsArray(Test_StackDefaultLength)    == OK);
  assert(TestStackPopAt()			      == OK);
  assert(TestStackPushAt()			      == OK);
  return 0;
}
