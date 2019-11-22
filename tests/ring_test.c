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
#include "ring.h"
#include "test.h"

void initDefaultRing(Ring** r)
{
  *r = NewRing(Test_DefaultLength);
  if(*r == nil) {
    fprintf(stderr, "NewRing() fail.\n");    
    abort();
  }
  for(uintmax_t i = 0; i < Test_DefaultLength; i++) {
    RingPush(*r, (uintptr_t)i);
  }
};

status TestRing(uintmax_t size)
{
  Data value;
  Ring* r;
  initDefaultRing(&r);
  value = RingPop(r);
#ifdef DEBUG
  printf("value(%lu)\n", value);
#endif
  RemoveRing(r);
  return OK;
};

int main()
{
  assert(TestRing(Test_DefaultLength) == OK);
  return 0;
};
