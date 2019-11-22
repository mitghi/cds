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

#ifndef _THREAD_H_
#include "thread.h"
#endif

#include "queue.h"

#define nil NULL
#define _cast
#define _ void* _discard
#define MAXBYTE 0xffffffffffffffff

/* SECTION: Utilities */
uintmax_t
setBit(uintmax_t num, uintmax_t index, bool state)
{
  return state ? num | (1 << (--index)):
    num ^ ((MAXBYTE^(1<<(--index)))^MAXBYTE);
};

uintmax_t
getBit(uintmax_t num, uintmax_t index)
{
  return num & (1 << index);
};

/* SECTION: (LOCAL) Forward declerations */
static Queue*
newQueue(void);

static Queue*
newConcurrentQueue(void);

static Node*
newNode(void);

static bool
queuePush(Queue*, Data);

static bool
queuePushAt(Queue*, int, Data);

static Node*
queuePop(Queue*);

static Data
queuePopValue(Queue*);

static Node*
queuePopAt(Queue*, int);

static Data
queuePopValueAt(Queue*, int);

static bool
queueContains(Queue*, Data);

static Node**
queueAsArray(Queue* q);

static uintmax_t
queueDataType(Queue*);

inline static void
nodeSetValue(Node*, Data);

inline static void
nodeSetNext(Node*, Node*);

inline static void
queueSetDataType(Queue*, uintmax_t);

/* SECTION: Internals */
inline static void
nodeSetValue(Node* n, Data value)
{
  n->value = value;
};

inline static void
nodeSetNext(Node* c, Node* n)
{
  c->next = n;
};

inline static void
queueSetDataType(Queue* q, uintmax_t datatype)
{
  q->datatype = datatype;
};

inline static void
queueSetNodeDeallocator(Queue *q, void (*nd)(Data))
{
  q->nd = nd;
};

static Node*
newNode(void)
{
  return _cast (Node*) alloc(sizeof(Node*));
};

static Queue*
newQueue(void)
{
  Queue* q = _cast (Queue*) alloc(sizeof(*q));
  if(q == nil){
    return nil;
  }
  memset(q, 0x00, sizeof(*q));
  q->head = _cast (Node**) alloc(sizeof(*q->head));
  if(q->head == nil){
    free(q->head);
    free(q);
    return nil;
  }
  *q->head = nil;
  q->tail = _cast (Node**) alloc(sizeof(*q->tail));
  if(q->tail == nil){
    free(q->head);
    free(q->tail);
    free(q);
    return nil;
  }
  *q->tail = nil;  
  return q;
};

static Queue*
newConcurrentQueue(void)
{
#ifdef _THREAD_H_
  Queue* q = newQueue();
  MutexInit(&q->lock);
  q->options = setBit(q->options, 1, true);
  return q;
#else
#pragma message "newConcurrentQueue(void) is not implemented."  
#endif

  return NULL;  
};

static bool
queuePush(Queue* q, Data value)
{
  Node* node = _cast (Node*) alloc(sizeof(*node));
  if(node == nil){
    return false;
  }
  node->value = value;
  node->next = nil;
  if(*q->head == *q->tail) {
    if(*q->head == nil) {
      *q->head = node;
      *q->tail = *q->head;
      goto OK;
    }
  }
  (*q->tail)->next = node;
   *q->tail = node;
 OK:
   q->size++;
   return true;
};

static Node*
queuePop(Queue* q)
{
  Node* n = nil;
  if(*q->head == nil) {
    return nil;
  }
  if(*q->head == *q->tail) {
    n = *q->head;
    *q->head = nil;
    *q->tail = nil;
  } else if(*q->head != nil) {
    n = *q->head;
    *q->head = (*q->head)->next;
  }
  q->size--;
  return n;
};

static Data
queuePopValue(Queue* q)
{
  Node* n = queuePop(q);
  Data retval = 0x0;
  if(n == nil) {
    return 0x0;
  }
  retval = n->value;
  free(n);
  return retval;
};

static void
removeQueue(Queue* q)
{
  Node *head = *q->head;
  if(head == nil) {
    goto DEALLOC;
  }  
  while(head != nil){
    Node *current = head;
    head = head->next;
    free(current);
  }
 DEALLOC:
  if(q->head != nil) {
    free(q->head);
  }
  if(q->tail != nil) {
    free(q->tail);
  }
  free(q);
};

static void
resetQueue(Queue* q)
{
  Node* head = *q->head;
  if(head == nil) {
    goto DEALLOC;
  }  
  while(head != nil) {
    Node* current = head;
    head = head->next;
    free(current);
  }
 DEALLOC:  
  *q->head = nil;
  *q->tail = nil;
  q->size = 0;
};

// queueReverse reverses the queue.
static void
queueReverse(Queue* q, Node* n)
{
  if(n == nil) {
    return;
  }
  static uintmax_t depth = 0;
  depth++;
  queueReverse(q, (n->next));
  depth--;
  if(n->next != nil) {
    n->next->next = n;
  } else {
    *(q->head) = n;
  }
  if(depth == 0) {
    n->next = nil;
  }
};

// queuePopAt pops element at the given
// index, otherwise `NULL`.
static Node*
queuePopAt(Queue* q, int index)
{
  if(q == nil) {
    return nil;
  } else if(index == 0) {
    return queuePop(q);
  }
  bool isReverse = (index < 0);
  Node* result = nil;
  Node* current = nil;
  if(isReverse) {
    index = -index;
    QueueReverse(q);
  }
  current = *q->head;
  while(current != nil) {
    if(index == 1) {
      result = current->next;
      if(result != nil) {
	current->next = result->next;
	result->next = nil;	
      }
      break;
    }
    current = current->next;
    index--;    
  }
  if(isReverse) {
    QueueReverse(q);
  }
  if(result != nil) {
    q->size--;
  }
  return result;
};

static Data
queuePopValueAt(Queue* q, int index)
{
  Node* n = queuePopAt(q, index);
  if(n == nil) {
    return 0x0;
  }
  Data d = n->value;
  free(n);
  return d;
};

static bool
queueContains(Queue* q, Data value)
{
  Node** current;
  current = q->head;
  for(current = q->head; *current != nil; current = &((*current)->next)) {
    if((*current)->value == value) {
      return true;
    }
  }
  return false;
};

static bool
queueRemoveAt(Queue* q, int index)
{
  Node* n = queuePopAt(q, index);
  if(n == nil) {
    return false;
  }
  if(q->nd) {
    q->nd(n->value);
  }
  free(n);
  return true;
};

static bool
queuePushAt(Queue* q, int index, Data value)
{
  if(q == nil) {
    return false;
  } else if(index == 0) {
    return queuePush(q, value);
  }
  Node* n = newNode();
  if(n == nil) {
    free(n);
    return false;
  }
  nodeSetValue(n, value);
  bool isReverse = (index < 0);
  bool isSuccessfull = false;
  Node* current = nil;
  if(isReverse) {
    index = -index;
    QueueReverse(q);
  }
  current = *q->head;
  while(current != nil) {
    if(index == 1) {
      if(current->next != nil) {
	nodeSetNext(n, current->next);
	current->next = n;
      }
      isSuccessfull = true;
      q->size++;
      break;
    }
    current = current->next;
    index--;
  }
  if(isReverse) {
    QueueReverse(q);
  }
  return isSuccessfull;
};

static Node**
queueAsArray(Queue* q)
{
  if(0 == q->size) {
    return nil;
  }
  uintmax_t counter = 0;
  Node** nodes = _cast (Node**) alloc(sizeof(Node**)*q->size);
  Node* current = *q->head;
  while(current != nil) {
    nodes[counter] = current;
    current = current->next;
    counter++;
  }
  return nodes;
};

static uintmax_t
queueDataType(Queue* q)
{
  return q->datatype;
};

/* SECTION: API */

Queue*
NewQueue(void)
{
  return newQueue();
};

Queue*
NewConcurrentQueue(void)
{
  return newConcurrentQueue();
};

bool
QueuePush(Queue* const q, Data value)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    bool retval = queuePush(q, value);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif

  return queuePush(q, value);
};

bool
QueuePushAt(Queue* const q, int index, Data value)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    bool retval = queuePushAt(q, index, value);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif
  
  return queuePushAt(q, index, value);
};

Node*
QueuePop(Queue* const q)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    Node* retval = queuePop(q);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif

  return queuePop(q);
};

Data
QueuePopValue(Queue* const q)
{
  Data retval;
  #ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    retval = queuePopValue(q);
    MutexUnlock(&q->lock);
    return retval;
  }
  #endif
  
  retval = queuePopValue(q);  
  return retval;
};

Node*
QueuePopAt(Queue* const q, const int index)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    Node* retval = queuePopAt(q, index);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif

  return queuePopAt(q, index);
};


Data
QueuePopValueAt(Queue* const q, const int index)
{
  Data retval;
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    retval = queuePopValueAt(q, index);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif

  retval = queuePopValueAt(q, index);
  return retval;
};

bool
QueueContains(Queue* const q, Data const d)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    bool retval = queueContains(q, d);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif

  return queueContains(q, d);
};

Node**
QueueAsArray(Queue* const q)
{
  Node** retval;
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    retval = queueAsArray(q);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif
  
  retval = queueAsArray(q);
  return retval;
};

bool
QueueRemoveAt(Queue* const q, const int index)
{
  bool retval;
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    retval = queueRemoveAt(q, index);
    MutexUnlock(&q->lock);
    return retval;
  }
#endif
  
  retval = queueRemoveAt(q, index);
  return retval;
};

bool
QueueIsEmpty(Queue* const q)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    bool retval = q->size;    
    MutexUnlock(&q->lock);
    return retval == 0;
  }
#endif
  return q->size == 0;
};


uintmax_t
QueueDataType(Queue* const q)
{
  return queueDataType(q);
};

void
RemoveQueue(Queue* q)
{
  removeQueue(q);
};

void
ResetQueue(Queue* const q)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    resetQueue(q);
    MutexUnlock(&q->lock);
    return;
  }  
#endif

  resetQueue(q);
};

void
QueueReverse(Queue* const q)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    queueReverse(q, *q->head);        
    MutexUnlock(&q->lock);
    return;
  }  
#endif
  
  queueReverse(q, *q->head);
};


void
QueueSetDataType(Queue* const q, uintmax_t datatype)
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    queueSetDataType(q, datatype);
    MutexUnlock(&q->lock);
    return;
  }  
#endif
  
    queueSetDataType(q, datatype);
};

void
QueueSetNodeDeallocator(Queue* const q, void (*nd)(Data))
{
#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);
    queueSetNodeDeallocator(q, nd);
    MutexUnlock(&q->lock);
    return;
  }  
#endif
  
  queueSetNodeDeallocator(q, nd);
};

int
QueueSize(Queue* const q)
{

#ifdef _THREAD_H_
  if(getBit(q->options, 1) == QUEUECONCURRENT) {
    MutexLock(&q->lock);    
    int retval = q->size;
    MutexUnlock(&q->lock);
    return retval;
  }  
#endif
  
  return q->size;
};

