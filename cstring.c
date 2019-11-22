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
#include <stdbool.h>
#include <stddef.h>
#include "cstring.h"

#define nil NULL
#define _cast

/* SECTION: vendor */
// Copyright (c) 2014 TingPing ustring(https://github.com/TingPing/ustring/)

/** 
 * Returns the length in bytes of a utf8 character.
 */
inline size_t
ucharlen (const char *str)
{
    // Taken from glib
    static const char utf8_skip[256] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
    };

    if (str == NULL)
        return 0;

    return utf8_skip[*((const unsigned char *)str)];
}

/** 
 * Returns the number of utf8 characters in a string.
 */
size_t
ustrlen (const char *str)
{
    if (str == NULL)
        return 0;

    size_t len = 0;
    while (*str != '\0')
    {
        str += ucharlen (str);
        len++;
    }
    return len;
}

// Used by both ustrlcpy() and ustrlcat()
static inline size_t
utf8_copy (char *restrict dst, const char *restrict src, size_t n_bytes)
{
    const char *src_start = src;

    while (n_bytes != 0)
    {
        size_t char_len = ucharlen (src);

        // Doesn't fit, terminate
        if (n_bytes < (char_len + 1))
        {
            *dst = '\0';
            break;
        }

        if (char_len == 1)
        {
            *dst = *src;
            if (*src == '\0')
                break;
        }
        else
        {
            memcpy (dst, src, char_len);
        }

        dst += char_len;
        src += char_len;
        n_bytes -= char_len;
    }

    // Finish going through str to return the strlen
    while (*src++ != '\0');

    // Don't include terminator
    return (src - src_start - 1);
}

size_t
ustrlcat (char *restrict dst, const char *restrict src, size_t n_bytes)
{
    if (src == NULL)
        return 0;

    if (dst == NULL)
        return strlen (src);

    const char *dst_start = dst;
    while (n_bytes != 0 && *dst != '\0')
    {
        n_bytes--;
        dst++;
    }

    return (dst - dst_start + utf8_copy (dst, src, n_bytes));
}

size_t
ustrlcpy (char *restrict dst, const char *restrict src, size_t n_bytes)
{
    if (src == NULL)
        return 0;

    if (dst == NULL)
        return strlen (src);

    return utf8_copy (dst, src, n_bytes);
}


/* SECTION: (LOCAL) forward declerations */
static String*
newString(const char*);

static char*
stringMerge(char*, const char*);

static inline bool
stringIsEqual(const char*, const char*);

/* SECTION: Internals */
static String*
newString(const char* str)
{
  size_t allocsize = strlen(str) + 1;
  String* s = _cast (String*) alloc(sizeof(*s));
  if(s == nil) {
    return nil;
  }
  memset(s, 0x00, sizeof(*s));
  s->size = allocsize-1;
  s->buffer = _cast (char*) alloc(sizeof(char)*allocsize);
  if(s->buffer == nil) {
    goto ERROR;
  }
  s->buffer[allocsize-1] = '\0';
  ustrlcpy(s->buffer, str, allocsize);
  return s;
  
 ERROR:
  free(s);
  return nil;
};

static char*
stringMerge(char* lstr, const char* rstr)
{
  if(lstr == nil || rstr == nil) {
    return nil;
  }
  char* buffer;
  size_t allocsize = (strlen(lstr) + strlen(rstr)) + 1;
  buffer = _cast (char*) realloc(lstr, sizeof(char)*allocsize);
  if(buffer == nil) {
    return nil;
  }
  ustrlcat(buffer, rstr, allocsize);  
  return buffer;
};

static inline bool
stringIsEqual(const char* lstr, const char* rstr)
{
  return (strcmp(lstr, rstr) == 0);
};

/* SECTION: API */

String*
NewString(const char* str)
{
  return newString(str);
};

String*
StringClone(const String *str)
{
  if(str == nil) {
    return nil;
  }
  String* n = newString(str->buffer);
  return n;
};

String*
StringMerge(String* lstr, const String* rstr)
{
  if(lstr == nil || rstr == nil) {
    return nil;
  }
  char* buffer = stringMerge(lstr->buffer, rstr->buffer);
  if(buffer == nil) {
    return nil;
  } else if (buffer != lstr->buffer) {
    lstr->buffer = buffer;
  }
  lstr->size = strlen(buffer);
  return lstr;
};

String*
StringMergeC(String* lstr, const char* rstr)
{
  if(lstr == nil || rstr == nil) {
    return nil;
  }
  char* buffer = stringMerge(lstr->buffer, rstr);
  if(buffer == nil) {
    return nil;
  }
  lstr->size = strlen(buffer);
  lstr->buffer = buffer;
  return lstr;
};

bool
StringIsEqual(const String* lstr, const String* rstr)
{
  return stringIsEqual(lstr->buffer, rstr->buffer);
};

bool
StringIsEqualC(const String* lstr, const char* rstr)
{
  return stringIsEqual(lstr->buffer, rstr);
};

char*
StringCharAt(const String* str, uintmax_t index)
{
  // TODO
  // . add utf-8
  if(index > str->size) {
    return nil;    
  }
  return &str->buffer[index];  
};

inline void
StringPrint(const String* str)
{
  printf("%s\n", str->buffer);
};

inline uintmax_t
StringLen(String* str)
{
  return ustrlen(str->buffer);
};

void
FreeString(String* str)
{
  free(str->buffer);
  free(str);
};
