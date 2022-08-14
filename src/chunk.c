#include "chunk.h"
#include "memory.h"

static void _add_value(Chunk *chunk, Value value) {
	if (chunk->valuesCapacity <= chunk->valuesSize) {
		int oldCapacity = chunk->valuesCapacity;
		chunk->valuesCapacity = chunk->valuesCapacity < 8 ? 8 : chunk->valuesCapacity * 2;
		chunk->values = RESIZE_ARRAY(Value, chunk->values, oldCapacity, chunk->valuesCapacity);
	}

	chunk->values[chunk->valuesSize] = value;
	chunk->valuesSize++;
}

Chunk *chunk_create() {
	Chunk *chunk = ALLOCATE_STRUCT(Chunk);

	chunk->codeSize = 0;
	chunk->codeCapacity = 0;
	chunk->code = NULL;
	chunk->lines = NULL;

	chunk->valuesSize = 0;
	chunk->valuesCapacity = 0;
	chunk->values = NULL;

	return chunk;
}

void chunk_destroy(Chunk *chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->codeCapacity);
	FREE_ARRAY(int, chunk->lines, chunk->codeCapacity);
	FREE_ARRAY(Value, chunk->values, chunk->valuesCapacity);
	FREE_STRUCT(Chunk, chunk);
}

void chunk_add_byte(Chunk *chunk, uint8_t byte, int line) {
	if (chunk->codeCapacity <= chunk->codeSize) {
		int oldCapacity = chunk->codeCapacity;
		chunk->codeCapacity = chunk->codeCapacity < 8 ? 8 : chunk->codeCapacity * 2;
		chunk->code = RESIZE_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->codeCapacity);
		chunk->lines = RESIZE_ARRAY(int, chunk->lines, oldCapacity, chunk->codeCapacity);
	}

	chunk->code[chunk->codeSize] = byte;
	chunk->lines[chunk->codeSize] = line;
	chunk->codeSize++;
}

void chunk_add_word(Chunk *chunk, uint16_t word, int line) {
	chunk_add_byte(chunk, ((uint8_t *)&word)[0], 123);
	chunk_add_byte(chunk, ((uint8_t *)&word)[1], 123);
}

uint16_t chunk_add_constant(Chunk *chunk, Value value) {
	_add_value(chunk, value);
	return chunk->valuesSize - 1;
}
