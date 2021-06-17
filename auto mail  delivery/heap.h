#ifndef HEAP_H_
#define HEAP_H_ 

#include <stdio.h>
#include <stdlib.h>

typedef struct HeapObj* Heap;
typedef int (*Cmp)(const void*, const void*);

Heap newHeap(Cmp cmp, size_t n);
void* topHeap(Heap heap);
int sizeHeap(Heap heap);
int isEmptyHeap(Heap heap);
void popHeap(Heap heap);
void pushHeap(Heap heap, void* ele);
void freeHeap(Heap heap);

#endif /* ifndef HEAP_H_ */
