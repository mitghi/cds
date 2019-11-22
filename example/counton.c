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
#include "queue.h"
#include "stack.h"

#define nil NULL

typedef char* string;
typedef struct counton{
  Queue* queue;
  unsigned int count;
} counton;

int main(int argc, char* argv[])
{
  const string word = "exampleword";
  int wlen = strlen(word);
  string names[] = {"first", "second", "third", "fourth", "fifth", "sixth", NULL};  
  counton co;
  co.count = 0;  
  co.queue = NewQueue();
  for(string *w = names; *w != nil; w++) QueuePush(co.queue, (uintptr_t)*w);
  while(!QueueIsEmpty(co.queue)){
    /*  d e b u g  */
    // co.count += wlen; // starting from mth+1
    /*  d e b u g  */    
    co.count = wlen;
    Data d;
    int index = co.count % QueueSize(co.queue)-1;
    if(index == -1) {
      d = QueuePopValue(co.queue);
      printf("last: %s\n", (char*) d);
      break;
    }
    d = QueuePopValueAt(co.queue, index);
    printf("(%d) %s is out\n", index, (char*)d);
  }
  RemoveQueue(co.queue);
  return 0;
};
