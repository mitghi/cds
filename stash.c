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
#include <string.h>
#include <stdbool.h>

#define MAXBYTE 0xffffffffffffffff
#define _cast

typedef unsigned int uint;
typedef unsigned char uchar;
typedef char* string;
typedef struct string_t string_t;

uint
setBit(uint num, uint index, bool state, int mask)
{
  return mask&(state ? num | (1 << (--index)):
	  num ^ ((mask^(1<<(--index)))^mask));
};

uint
getBit(uint num, uint index)
{
  return num & (1 << index);
};

/* SECTION: Structs */

struct __attribute__ ((__packed__)) string_t {
  unsigned char flag;
  char buf[];
};

const size_t headerLen = sizeof(string_t);

string
newString(const void* str, unsigned int len)
{
  
  string retval;
  string block = _cast (string)malloc(headerLen+len+1);
  retval = block+headerLen-1;
  strcpy(retval, str);
  return retval;
};

string_t*
stringMeta(const string str)
{
  string_t* meta = (_cast (string_t*)(str-sizeof(string_t)));
  return meta;
};

void
stringFree(const string str)
{
  string_t* meta = _cast (string_t*)str-sizeof(string_t);
  free(meta);
};

int main()
{
  string str = newString("test", 4);
  string_t* strmeta = stringMeta(str);
  strmeta->flag = _cast (uchar) setBit(_cast (uint)strmeta->flag, 1, true, 0xf);
  printf("string:%s\tflag:%d\n",
	 str, _cast (uint)strmeta->flag);  
  return 0;
};
