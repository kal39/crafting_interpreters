#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

#define ALLOCATE_STRUCT(type) reallocate(NULL, 0, sizeof(type))
#define FREE_STRUCT(type, ptr) reallocate(ptr, sizeof(type), 0)

#define FREE_ARRAY(type, ptr, oldLen) reallocate(ptr, sizeof(type) * (oldLen), 0)
#define RESIZE_ARRAY(type, ptr, oldLen, newLen) reallocate(ptr, sizeof(type) * (oldLen), sizeof(type) * (newLen))

void *reallocate(void *pointer, size_t oldLen, size_t newSize);

#endif