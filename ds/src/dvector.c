#include <stdlib.h>                  /* malloc, realloc, free */
#include <assert.h>                  /* assert */
#include <string.h>                  /* memcpy */

#include "dvector.h"

#define INC_FACTOR(x) ((x * 1.5) + 1)
#define DEC_FACTOR(x) (x * 2/3)
#define SUCCESS (0)
#define FAILURE (1)

struct dvector {
    size_t size;
    size_t capacity;
    size_t element_size;
    void* array;
};

dvector_t* DvectorCreate(size_t capacity, size_t element_size)
{
    dvector_t* p_dvector = (dvector_t*)malloc(sizeof(dvector_t));
    p_dvector->array = malloc(capacity * element_size);

    if (NULL == p_dvector || NULL == p_dvector->array)
    {
        return NULL;
    }

    p_dvector->size = 0;
    p_dvector->capacity = capacity;
    p_dvector->element_size = element_size;

    return p_dvector;
}

void DvectorDestroy(dvector_t* dvector)
{
    free(dvector -> array);
    free(dvector);
}

size_t DvectorCapacity(const dvector_t* dvector)
{
    assert(NULL != dvector);
    
    return dvector->capacity;
}

size_t DvectorSize(const dvector_t* dvector)
{
    assert(NULL != dvector);
    
    return dvector->size;
}

void DvectorSetElement(dvector_t* dvector, size_t index, const void* value)
{
    unsigned char* p_array = (unsigned char*)(dvector -> array);
    
    assert(NULL != dvector);
    assert(index < dvector->size);

    memcpy(p_array + (index * dvector->element_size), value,
                                                        dvector-> element_size);
}

void DvectorGetElement(const dvector_t* dvector, size_t index, void* dest)
{
    unsigned char* p_array = (unsigned char*)(dvector->array);
    
    assert(NULL != dvector);
    assert(index < dvector -> size);

    memcpy(dest, p_array + (index * dvector->element_size),
                                                        dvector-> element_size);
}

int DvectorPushBack(dvector_t* dvector, const void* element)
{
    unsigned char* p_array;
    
    assert(NULL != dvector);

    if (dvector->size == dvector->capacity)
    {
        if (FAILURE == DvectorResize(dvector, INC_FACTOR(dvector->capacity)))
        {
            return FAILURE;
        }
    }

    p_array = (unsigned char*)(dvector->array);
    memcpy(p_array + (dvector->size * dvector->element_size), element,
                                                        dvector-> element_size);
    dvector->size++;
    
    return SUCCESS;
}

int DvectorPopBack(dvector_t* dvector)
{
    assert(NULL != dvector);

    if (!dvector->size)
    {
        return FAILURE;
    }

    dvector->size--;

    if (dvector->size <= DEC_FACTOR(dvector->capacity))
    {
        DvectorShrink(dvector);
    }
    
    return SUCCESS;
}

int DvectorResize(dvector_t* dvector, size_t new_capacity)
{    
    assert(NULL != dvector);

    dvector->array = realloc(dvector->array, new_capacity * dvector->element_size + 1);

    if (NULL == dvector->array)
    {
        return FAILURE;
    }
    
    dvector->capacity = new_capacity;
    dvector->size = dvector->size > dvector->capacity ? dvector->capacity : dvector->size;
    
    return SUCCESS;
}

int DvectorShrink(dvector_t* dvector)
{
    return DvectorResize(dvector, dvector->size);
}
