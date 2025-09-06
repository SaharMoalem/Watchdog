#include <stdlib.h>                  /* malloc, free */
#include <assert.h>                  /* assert */

#include "heap_pq.h"
#include "heap.h"

struct priority_queue
{
    heap_t* heap;
};

heap_pq_t* PQCreate(int (*compare_func)(const void*, const void*))
{
    heap_pq_t* pq = NULL;
    
    assert(compare_func);

    pq = (heap_pq_t*)malloc(sizeof(heap_pq_t));

    if (pq == NULL)
    {
        return NULL;
    }
    
    pq->heap = HeapCreate(compare_func);

    if (pq->heap == NULL)
    {
        free(pq);
        return NULL;
    }
    
    return pq;
}

void PQDestroy(heap_pq_t* pq)
{
    assert(pq);
    
    HeapDestroy(pq->heap);
    free(pq);
}

int PQEnqueue(heap_pq_t* pq, void* data)
{
    assert(pq);
    
    return HeapPush(pq->heap, data);
}

void* PQDequeue(heap_pq_t* pq)
{
    void* peek = NULL;

    assert(pq);
    assert(!PQIsEmpty(pq));
    
    peek = HeapPeek(pq->heap);
    HeapPop(pq->heap);
    
    return peek;
}

void* PQPeek(const heap_pq_t* pq)
{
    assert(pq);
    assert(!PQIsEmpty(pq));
    
    return HeapPeek(pq->heap);
}

int PQIsEmpty(const heap_pq_t* pq)
{
    assert(pq);
    
    return HeapIsEmpty(pq->heap);
}

size_t PQSize(const heap_pq_t* pq)
{
    assert(pq);
    
    return HeapSize(pq->heap);
}

void PQClear(heap_pq_t* pq)
{
    assert(pq);
    
    while(!PQIsEmpty(pq))
    {
        PQDequeue(pq);
    }
}

void* PQErase(heap_pq_t* pq, int (*is_match)(const void*, const void*),
                                                            const void* param)
{
    assert(pq);
    assert(is_match);

    return HeapRemove(pq->heap, (void*)param, is_match);
}
