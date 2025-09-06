#ifndef __DVECTOR_H__
#define __DVECTOR_H__

#include <stddef.h> /*size_t*/

typedef struct dvector dvector_t;

/* 
*  @desc:         Allocates dvector with @capacity where each element is @element_size in bytes. 
*  @params:       @capacity of dynamic vector (number of elements) and element_size (sizeof elements in bytes) 
*  @return value: function returns a pointer to stack if succeeded  or NULL if failed
*/
dvector_t* DvectorCreate(size_t capacity, size_t element_size);
/*
*   @Desc: Free the memory allocated for dvector_t
*   @Params: @dvector - pointer to a pre-allocated dvector_t data type
*/
void DvectorDestroy(dvector_t* dvector);

/*
*   @Desc: Return the capacity of the @dvector
*   @Params: @dvector  pointer to a pre-allocated dvector_t data type
*   @Return: Return the capacity of the @dvector
*/
size_t DvectorCapacity(const dvector_t* dvector);

/* 
*   @Desc: Returns the amount of elements currently stored in the dvector
*   @Params: @dvector pointer to a pre-allocated dvector_t data type
*   @Return: Returns the size of the @dvector
*/
size_t DvectorSize(const dvector_t* dvector);
/*
*   @Desc: Set specific index to a value
*   @Params: pointer to a pre-allocated dvector_t data type, index, void* value to set in a specific index .
*   @Return: Void function 
*/
void DvectorSetElement(dvector_t* dvector, size_t index, const void* value);
/*
*   @Desc: Get the value of specific index
*   @Params: pointer to a pre-allocated dvector_t data type, index, void* to the return element.
*   @Return: Void function 
*/
void DvectorGetElement(const dvector_t* dvector, size_t index, void* dest);
/*
*   @Desc: Push the final element in the dvector
*   @Params: Pointer to a pre-allocated dvector_t data type, void* to the return element.
*   @Return: Return success (0) or failure (1)
*/
int DvectorPushBack(dvector_t* dvector, const void* element);

/*
*   @Desc: Pop out the element at size index in dvector
*   @Params: Pointer to a pre-allocated dvector_t data type.
*   @Return: Return success (0) or failure (1). If this element is required use DvectorGetElement before.
*/

int DvectorPopBack(dvector_t* dvector);

/*
*   @Desc: Update the capacity of the dvector
*   @Params: pointer to a pre-allocated dvector_t data type, size_t new capacity for the dvector
*   @Return: (0) if success or (1) for failure
*/

int DvectorResize(dvector_t* dvector, size_t new_capacity);
/*
*   @Desc: shrink to size
*   @Params: pointer to a pre-allocated dvector_t data type
*   @Return: (0) if success or (1) for failure
*/
int DvectorShrink(dvector_t* dvector);

#endif  /*End of header guard Dvector*/ 
