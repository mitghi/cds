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

#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>
#include <stdbool.h>

/*  SECTION: Types  */
typedef struct cstring String;

/*  SECTION: Structs  */
struct cstring {
  char *buffer;
  int cursor, size;
};

/* SECTION: Allocators */
#define alloc malloc

/* SECTION: Vendor */
size_t ucharlen (const char *str) __attribute__((pure));
size_t ustrlen (const char *str) __attribute__((pure));
size_t ustrlcat (char *restrict dst, const char *restrict src, size_t n_bytes);
size_t ustrlcpy (char *restrict dst, const char *restrict src, size_t n_bytes);

/* SECTION: Prototypes */
String*
NewString(const char*);

String*
StringClone(const String *);

String*
StringMerge(String*, const String*);

String*
StringMergeC(String*, const char*);

bool
StringIsEqual(const String*, const String*);

bool
StringIsEqualC(const String*, const char*);

char*
StringCharAt(const String*, uintmax_t);

uintmax_t
StringLen(String*);

void
StringPrint(const String*);

void
FreeString(String*);

#endif
