#include "memory.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
	if (newSize == 0) {
		free(pointer);
		return NULL;
	} else {
		void *result = realloc(pointer, newSize);
		if (result == NULL) {
			fprintf(stderr, "realloc failed\n");
			exit(1);
		}
		return result;
	}
}