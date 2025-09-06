#include <stdlib.h>     /*malloc, free*/
#include <assert.h>     /* assert */

#include "heap.h"
#include "dvector.h"

#define CAPACITY (1024)
#define ELEM_SIZE (sizeof(void*))
#define GET_LEFT(index) (index * 2 + 1)
#define GET_RIGHT(index) (GET_LEFT(index) + 1)
#define GET_PARENT(index) ((index - 1) / 2)

struct heap {
    dvector_t* vector;
    compare_func_t compare_func;
};

heap_t* HeapCreate(compare_func_t compare)
{
    heap_t* heap = NULL;

    assert(compare);

    heap = (heap_t*)malloc(sizeof(heap_t));

    if(!heap)
    {
        return NULL;
    }

    heap->vector = DvectorCreate(CAPACITY, ELEM_SIZE);

    if(!heap->vector)
    {
        free(heap);
        return NULL;
    }

    heap->compare_func = compare;

    return heap;
}

void HeapDestroy(heap_t* heap)
{
    assert(heap);

    DvectorDestroy(heap->vector);
    free(heap);
}

static void Swap(dvector_t* vector, size_t index1, size_t index2)
{
    void* p_index1 = NULL;
    void* p_index2 = NULL;

    DvectorGetElement(vector, index1, &p_index1);
    DvectorGetElement(vector, index2, &p_index2);
    DvectorSetElement(vector, index1, &p_index2);
    DvectorSetElement(vector, index2, &p_index1);
}

static void HeapifyUp(heap_t* heap, size_t index)
{
    void* child = NULL;
    void* parent = NULL;

    if(index == 0)
    {
        return;
    }

    DvectorGetElement(heap->vector, index, &child);
    DvectorGetElement(heap->vector, GET_PARENT(index), &parent);

    if(heap->compare_func(parent, child) > 0)
    {
        Swap(heap->vector, index, GET_PARENT(index));
        HeapifyUp(heap, GET_PARENT(index));
    }
}

static void HeapifyDown(heap_t* heap, size_t index)
{
    size_t min_index = index;
    void* left = NULL;
    void* right = NULL;
    void* parent = NULL;
    void* min_child = NULL;

    if(GET_LEFT(index) >= DvectorSize(heap->vector))
    {
        return;
    }

    DvectorGetElement(heap->vector, index, &parent);
    DvectorGetElement(heap->vector, GET_LEFT(index), &left);
        
    if(GET_RIGHT(index) < DvectorSize(heap->vector))
    {
        DvectorGetElement(heap->vector, GET_RIGHT(index), &right);
    }

    min_child = parent;

    if(heap->compare_func(left, min_child) < 0)
    {
        min_child = left;
        min_index = GET_LEFT(index);
    }

    if(right && heap->compare_func(right, min_child) < 0)
    {
        min_index = GET_RIGHT(index);
    }

    if(min_index != index)
    {
        Swap(heap->vector, index, min_index);
        HeapifyDown(heap, min_index);
    }  
}

int HeapPush(heap_t* heap, void* data)
{
    assert(heap);

    if(DvectorPushBack(heap->vector, &data))
    {
        return 1;
    }

    HeapifyUp(heap, DvectorSize(heap->vector) - 1);

    return 0;
}

int HeapPop(heap_t* heap)
{
    assert(heap);
    assert(!HeapIsEmpty(heap));

    Swap(heap->vector, 0, DvectorSize(heap->vector) - 1);

    if(DvectorPopBack(heap->vector))
    {
        return 1;
    }

    if(!HeapIsEmpty(heap))
    {
        HeapifyDown(heap, 0);
    }

    return 0;
}

static void* FindElementToRemove(heap_t* heap, void* param,
                                            is_match_t is_match, size_t index)
{
    void* index_data = NULL;

    if(index >= DvectorSize(heap->vector))
    {
        return NULL;
    }

    DvectorGetElement(heap->vector, index, &index_data);

    if(is_match(index_data, param))
    {
        Swap(heap->vector, index, DvectorSize(heap->vector) - 1);
        DvectorPopBack(heap->vector);
        HeapifyDown(heap, index);
        return index_data;
    }

    index_data = FindElementToRemove(heap, param, is_match, GET_LEFT(index));

    if(index_data)
    {
        return index_data;
    }

    return FindElementToRemove(heap, param, is_match, GET_RIGHT(index));
}

void* HeapRemove(heap_t* heap, void* param, is_match_t is_match)
{
    assert(heap);
    assert(is_match);

    return FindElementToRemove(heap, param, is_match, 0);
}

size_t HeapSize(const heap_t* heap)
{
    assert(heap);

    return DvectorSize(heap->vector);
}

int HeapIsEmpty(const heap_t* heap)
{
    assert(heap);

    return DvectorSize(heap->vector) == 0;
}

void* HeapPeek(const heap_t* heap)
{
    void* dst = NULL;

    assert(heap);
    assert(!HeapIsEmpty(heap));

    DvectorGetElement(heap->vector, 0, &dst);

    return dst;
}

