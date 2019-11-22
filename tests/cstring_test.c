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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cstring.h"
#include "test.h"

#define isutf(c) (((c)&0xC0)!=0x80)

#define CHECKNIL(v)                         \
  if(v == nil) {                            \
    fprintf(stderr, "NewString() fail.\n");	\
    return FAIL;                            \
  }                                         \

status TestString(void)
{
  const char* str = "test";
  String *s = NewString(str);
  CHECKNIL(s);
#ifdef DEBUG
  StringPrint(s);
#endif
  if(s->size != strlen(str)) {
    FreeString(s);
    return FAIL;
  }
  FreeString(s);
  return OK;
};

status TestStringMerge(void)
{
  const char* expect = "test stringü.";
  String *s = NewString("test");
  String *n = NewString(" stringü.");
  CHECKNIL(s);
  CHECKNIL(n);
  s = StringMerge(s, n);
#ifdef DEBUG
  StringPrint(s);
  printf("length(s):%ju\tlength(expect):%ju\ts == expect -> %s\n", StringLen(s), ustrlen(expect), StringIsEqualC(s, expect) == 1? "true":"false");
#endif
  if(strcmp(s->buffer, expect) != 0) {
    goto ERROR;
  }
  FreeString(s);
  FreeString(n);
  return OK;
  
 ERROR:
  FreeString(s);
  FreeString(n);
  return FAIL;
};

int main()
{
  assert(TestString()	   == OK);
  assert(TestStringMerge() == OK);
};
