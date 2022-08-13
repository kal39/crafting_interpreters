#include <stdlib.h>

#include "memory.h"

void *reallocate(void *pointer, size_t size) {
	if (size == 0) {
		free(pointer);
		return NULL;
	} else {
		void *result = realloc(pointer, size);
		if (result == NULL) exit(1);
		return result;
	}
}