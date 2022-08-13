#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

static void _add_value(Chunk *chunk, Value value) {
	if (chunk->valuesCapacity <= chunk->valuesSize) {
		chunk->valuesCapacity = chunk->valuesCapacity < 8 ? 8 : chunk->valuesCapacity * 2;
		chunk->values = reallocate(chunk->values, chunk->valuesCapacity * sizeof(Value));
	}

	chunk->values[chunk->valuesSize] = value;
	chunk->valuesSize++;
}

void chunk_initialize(Chunk *chunk) {
	chunk->codeSize = 0;
	chunk->codeCapacity = 0;
	chunk->code = NULL;
	chunk->lines = NULL;

	chunk->valuesSize = 0;
	chunk->valuesCapacity = 0;
	chunk->values = NULL;
}

void chunk_destroy(Chunk *chunk) {
	reallocate(chunk->code, 0);
	reallocate(chunk->lines, 0);
	reallocate(chunk->values, 0);
}

void chunk_add_byte(Chunk *chunk, uint8_t byte, int line) {
	if (chunk->codeCapacity <= chunk->codeSize) {
		chunk->codeCapacity = chunk->codeCapacity < 8 ? 8 : chunk->codeCapacity * 2;
		chunk->code = reallocate(chunk->code, chunk->codeCapacity * sizeof(uint8_t));
		chunk->lines = reallocate(chunk->lines, chunk->codeCapacity * sizeof(int));
	}

	chunk->code[chunk->codeSize] = byte;
	chunk->lines[chunk->codeSize] = line;
	chunk->codeSize++;
}

uint16_t chunk_add_constant(Chunk *chunk, Value value) {
	_add_value(chunk, value);
	return chunk->valuesSize - 1;
}
