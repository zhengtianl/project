#include "heap.h"

typedef struct HeapObj {
    void** elems;
    size_t cap;
    int size;
    Cmp cmp;
} HeapObj;

Heap newHeap(Cmp cmp, size_t n) {
    Heap heap = (Heap)malloc(sizeof(HeapObj));
    heap->cmp = cmp;
    heap->cap = n + 1;
    heap->size = 0;
    heap->elems = (void**)malloc(heap->cap * sizeof(void*));
    return heap;
}

static void swap(Heap heap, int i, int j) {
    void* tmp = heap->elems[j];
    heap->elems[j] = heap->elems[i];
    heap->elems[i] = tmp;
}

static void heapify(Heap heap, int i) {
    while (1) {
        int l = i << 1;
        int r = (i << 1) + 1;
        int largest = i;
        if (l <= heap->size &&
            heap->cmp(heap->elems[l],
                      heap->elems[i]) > 0) {
            largest = l;
        }
        if (r <= heap->size &&
            heap->cmp(heap->elems[r],
                      heap->elems[largest]) > 0) {
            largest = r;
        }

        if (largest != i) {
            swap(heap, largest, i);
            i = largest;
        } else {
            break;
        }
    }
}

void* topHeap(Heap heap) {
    return heap->elems[1];
}

int sizeHeap(Heap heap) {
    return heap->size;
}

int isEmptyHeap(Heap heap) {
    return heap->size <= 0;
}

void popHeap(Heap heap) {
    heap->elems[1] = heap->elems[heap->size];
    heap->size -= 1;
    heapify(heap, 1);
}

static void incCapacity(Heap heap) {
    heap->cap <<= 1;
    heap->elems = (void**)realloc(
            heap->elems,
            heap->cap * sizeof(void*));
}

void pushHeap(Heap heap, void* ele) {
    heap->size += 1;
    if (heap->size >= heap->cap) {
        incCapacity(heap);
    }
    heap->elems[heap->size] = ele;
    int i = heap->size;
    while (i > 1 &&
           heap->cmp(heap->elems[i / 2],
                     heap->elems[i]) < 0) {
        swap(heap, i / 2, i);
        i = i / 2;
    }
}

void freeHeap(Heap heap) {
    while (!isEmptyHeap(heap)) {
        topHeap(heap);
        popHeap(heap);
    }
    free(heap->elems);
    free(heap);
}
