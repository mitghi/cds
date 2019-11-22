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

#ifndef _THREAD_H_
#include "thread.h"
#endif

#include "stack.h"

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

/* SECTION: (LOCAL) forward declerations */
static Stack*
newStack(void);

static Stack*
newConcurrentStack(void);

static struct node*
newNode(void);

static bool
stackPush(Stack*, Data);

static bool
stackPushAt(Stack*, int, Data);

static struct node*
stackPop(Stack*);

static Node*
stackPopAt(Stack*, int);

static Data
stackPopValue(Stack* const);

static Data
stackPopValueAt(Stack*, int);

static bool
stackRemoveAt(Stack*, int);

static bool
stackContains(Stack*, Data);

static void
removeStack(Stack*);

static void
resetStack(Stack*);

// setters
inline static void
nodeSetValue(Node*, int);

inline static void
nodeSetNext(Node*, Node*);

inline static void
stackSetNodeDeallocator(Stack*, void (*)(Data));

inline static void
stackSetDataType(Stack*, uintmax_t);

// getters
static uintmax_t
stackDataType(Stack*);

/* SECTION: Internals */
static Stack*
newStack(void)
{
  Stack* l = _cast (Stack*) alloc(sizeof(*l));
  if(l == nil) {
    return nil;
  }
  _ = memset(l, 0x0, sizeof(*l));
  if(_discard == nil) {
    free(l);
    return nil;
  }
  l->head = _cast (Node**) alloc(sizeof(void**));
  if(l->head == nil) {
    goto ERROR;
  }
  *(l->head) = nil;
  return l;

 ERROR:
  free(*(l->head));
  free(l->head);
  free(l);
  return nil;
};

static Stack*
newConcurrentStack(void)
{
#ifdef _THREAD_H_
  Stack* l = newStack();
  l->options = setBit(l->options, 1, true);
  MutexInit(&l->lock);
  return l;
#else
#pragma message "newConcurrentStack(void) is not implemented."
#endif
  return NULL;
};

static struct node*
newNode(void)
{
  struct node* n = _cast (Node*) alloc(sizeof(*n));
  if(n == nil) {    
    free(n);
    return nil;
  }
  n->next = nil;
  return n;
};

inline static void
nodeSetValue(Node *n, int value)
{
  n->value = value;
};

inline static void
nodeSetNext(Node *n, Node *next)
{
  n->next = next;
};

inline static void
stackSetDataType(Stack* l, uintmax_t datatype)
{
  l->datatype = datatype;
};

static bool
stackPush(Stack* l, Data d)
{
  struct node* n = newNode();
  if(n == nil) {
    if(l->nd != nil) {
      l->nd(n->value);
    }
    free(n);
    return false;
  }
  nodeSetValue(n, d);
  nodeSetNext(n, *l->head);
  *l->head = n;
  l->size++;
  return true;
};

static struct node*
stackPop(l)
     Stack* l;
{
  if(*l->head == nil) {
    return nil;
  }
  struct node *n = *l->head;
  *(l->head) = (*(l->head))->next;
  n->next = nil;
  return n;
};

static Node*
stackPopAt(Stack* l, int index)
{
  if(l == nil) {
    return nil;
  } else if(index == 0) {
    l->size--;
    return stackPop(l);
  }
  bool isReverse = (index < 0);
  Node* result = nil;
  Node* current = nil;
  if(isReverse) {
    index = -index;
    StackReverse(l);
  }
  current = *l->head;
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
    StackReverse(l);
  }
  l->size--;
  return result;
};

static Node*
stackItemAt(Stack* l, int index)
{
  if(l == nil) {
    return nil;
  } else if(index == 0) {
    return *l->head;
  }
  bool isReverse = (index < 0);
  Node* current = nil;
  if(isReverse) {
    index = -index;
    StackReverse(l);
  }
  current = *l->head;
  while(current != nil && index !=0) {
    current = current->next;
    index--;
  }
  if(isReverse) {
    StackReverse(l);
  }
  return current;
};

// stackContains ?
static bool
stackContains(Stack* l, Data value)
{
  Node** current;
  current = l->head;
  for(current = l->head; *current != nil; current = &((*current)->next)) {
    if((*current)->value == value) {
      return true;
    }
  }
  return false;
};

// StackReverse reverses the stack.
void
stackReverse(Stack* l, Node* n)
{
  if(n == nil) {
    return;
  }
  static unsigned int depth = 0;    
  depth++;
  stackReverse(l, (n->next));
  depth--;
  if(n->next != nil) {
    n->next->next = n;
  } else {
    *(l->head) = n;
  }
  if(depth == 0) {
    n->next = nil;
  }
};

static bool
stackPushAt(Stack* l, int index, Data value)
{
  if(l == nil) {
    return false;
  } else if(index == 0) {
    l->size++;
    return stackPush(l, value);
  }
  struct node* n = newNode();
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
    StackReverse(l);
  }
  current = *l->head;
  while(current != nil) {
    if(index == 1) {
      if(current->next != nil) {
	nodeSetNext(n, current->next);	
	current->next = n;
      }
      isSuccessfull = true;
      l->size++;
      break;
    }
    current = current->next;
    index--;
  }
  if(isReverse) {
    StackReverse(l);
  }
  return isSuccessfull;
};

static void
removeStack(Stack* l)
{
  if(l->head == nil || (l->head != nil && *(l->head) == nil)) {
    free(l->head);
    free(l);
    return;
  }
  struct node *n = nil;
  while(*(l->head) != nil) {
    n = *(l->head);
    *(l->head) = (*(l->head))->next;
    n->next = nil;
    if(l->nd != nil) {      
      l->nd(n->value);
    }
    free(n);
  }
  free(l->head);
  free(l);
};

static void
resetStack(Stack* l)
{
  if(l->head == nil || (l->head != nil && *(l->head) == nil)) {
    free(l->head);
    return;
  }
  struct node *n = nil;
  while(*(l->head) != nil) {
    n = *(l->head);
    *(l->head) = (*(l->head))->next;
    n->next = nil;
    free(n);
  }
  free(l->head);  
};

inline static void
stackSetNodeDeallocator(Stack *l, void (*nd)(Data))
{
  l->nd = nd;
};

static uintmax_t
stackDataType(Stack* s)
{
  return s->datatype;
};

static Data
stackPopValue(Stack* const s)
{
  Node* n = stackPop(s);
  if(n == nil) {
    return 0x0;
  }
  Data d = n->value;
  free(n);
  return d;
};


static Data
stackPopValueAt(Stack* s, int index)
{
  Node* n = stackPopAt(s, index);
  if(n == nil) {
    return 0x0;
  }
  Data d = n->value;
  free(n);
  return d;
};

static bool
stackRemoveAt(Stack* s, int index)
{
  Node* n = stackPopAt(s, index);
  if(n == nil) {
    return false;
  }
  if(s->nd != nil) {
    s->nd(n->value);
  }
  free(n);
  return true;
};

/*  SECTION: API */

// NewStack allocates a new `Stack*`
// on heap. It returns `NULL` pointer, to
// indicate allocation failure.
// NOTE: This function wraps static
// newStack(...).
Stack*
NewStack(void)
{
  return newStack();
};

// NewConcurrentStack allocates a new `Stack*`
// on heap. It returns `NULL` pointer, to
// indicate allocation failure.
// NOTE: This function wraps static
// newConcurrentStack(...).
Stack*
NewConcurrentStack(void)
{
  return newConcurrentStack();  
};

// StackPush pushes a new heap resident
// `struct node*` with value `d`. It 
// returns 'bool false', to indicate failure.
bool
StackPush(Stack* l, Data d)
{
#ifdef _THREAD_H_
  MutexLock(&l->lock);
  bool result = stackPush(l, d);  
  MutexUnlock(&l->lock);
  return result;
#endif

  return stackPush(l, d);
};

// StackPop returns first item (
// stack insertion order ).
Node*
StackPop(Stack* l)
{
#ifdef _THREAD_H_
  MutexLock(&l->lock);
  struct node* n = stackPop(l);
  MutexUnlock(&l->lock);
  return n;
#endif

  return stackPop(l);
};

// RemoveStack releases the chain
// and `Stack* l`.
inline void
RemoveStack(Stack* l)
{
  removeStack(l);
};

// ResetStack releases the chain.
void
ResetStack(Stack* l)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    resetStack(l);
    MutexUnlock(&l->lock);
    return;
  }
#endif

  resetStack(l);
};

// StackPopAt pops element at the given
// index, otherwise `NULL`.
Node*
StackPopAt(Stack* l, int index)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    Node* retval = stackPopAt(l, index);
    MutexUnlock(&l->lock);
    return retval;
  }
#endif

  return stackPopAt(l, index);
};

// StackPushAt inserts heap resident
// `struct node*` with `value` at `index`.
// It returns `bool false` to indicate failure.
bool
StackPushAt(Stack* l, int index, Data value)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    bool retval = stackPushAt(l, index, value);
    MutexUnlock(&l->lock);
    return retval;
  }
#endif

  return stackPushAt(l, index, value);
};

Node*
StackItemAt(Stack* l, int index)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    Node* retval = stackItemAt(l, index);
    MutexUnlock(&l->lock);
    return retval;
  }
#endif

  return stackItemAt(l, index);
};

// StackReverse, reverse. It returns
// `bool false` when the given `Stack *l`
// is empty.
bool
StackReverse(Stack* l)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    if(*(l->head) == nil) {
      MutexUnlock(&l->lock);
      return false;
    }
    stackReverse(l, (*l->head));
    MutexUnlock(&l->lock);
    return true;
  }
#endif
  if (*(l->head) != nil) {
    stackReverse(l, (*l->head));
    return true;
  }
  return false;
};

// StackContains returns whether `value`
// is an element of `Stack* l`.
bool
StackContains(Stack* l, Data value)
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    bool retval = stackContains(l, value);
    MutexUnlock(&l->lock);
    return retval;
  }
#endif

  return stackContains(l, value);
};

// StackSetNodeDeallocator sets function pointer `*nd`
// as deallocator of `Node*` data.
void
StackSetNodeDeallocator(Stack *l, void (*nd)(Data))
{
#ifdef _THREAD_H_
  if(getBit(l->options, 1) == STACKCONCURRENT) {
    MutexLock(&l->lock);
    stackSetNodeDeallocator(l, nd);   
    MutexUnlock(&l->lock);
    return;
  }
#endif

  stackSetNodeDeallocator(l, nd);
};

bool
StackIsEmpty(Stack* s)
{
#ifdef _THREAD_H_
  if(getBit(s->options, 1) == STACKCONCURRENT) {
    MutexLock(&s->lock);
    bool retval = s->size;    
    MutexUnlock(&s->lock);
    return retval == 0;
  }
#endif
  
  return s->size == 0;
};

void
StackSetDataType(Stack* l, uintmax_t datatype)
{
  stackSetDataType(l, datatype);
};

uintmax_t
StackDataType(Stack* s)
{
  return stackDataType(s);
};

Node**
StackAsArray(Stack* s)
{
  if(0 == s->size) {
    return nil;
  }
  uintmax_t counter = 0;
  Node** nodes = _cast (Node**) alloc(sizeof(Node**)*s->size);
  Node* current = *s->head;
  while(current != nil) {
    nodes[counter] = current;
    current = current->next;
    counter++;
  }
  return nodes;
};

Data
StackPopValue(Stack* const s)
{
  Data retval;
#ifdef _THREAD_H_
  if(getBit(s->options, 1) == STACKCONCURRENT) {
    MutexLock(&s->lock);
    retval = stackPopValue(s);
    MutexUnlock(&s->lock);
    return retval;
  }
#endif

  retval = stackPopValue(s);
  return retval;
};

Data
StackPopValueAt(Stack* const s, int index)
{
  Data retval;
#ifdef _THREAD_H_
  if(getBit(s->options, 1) == STACKCONCURRENT) {
    MutexLock(&s->lock);
    retval = stackPopValueAt(s, index);
    MutexUnlock(&s->lock);
    return retval;
  }
#endif

  retval = stackPopValueAt(s, index);
  return retval;
};

bool
StackRemoveAt(Stack* const s, int index)
{
  bool retval;
#ifdef _THREAD_H_
  if(getBit(s->options, 1) == STACKCONCURRENT) {
    MutexLock(&s->lock);
    retval = stackRemoveAt(s, index);
    MutexUnlock(&s->lock);
    return retval;
  }
#endif

  retval = stackRemoveAt(s, index);
  return retval;
};

int
StackSize(Stack* const s)
{
#ifdef _THREAD_H_
  if(getBit(s->options, 1) == STACKCONCURRENT) {
    MutexLock(&s->lock);
    int retval = s->size;
    MutexUnlock(&s->lock);
    return retval;
  }
#endif

  return s->size;
};
