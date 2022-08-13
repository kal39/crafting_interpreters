#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"

typedef enum OpCode {
	OP_CONSTANT,
	OP_RETURN,
} OpCode;

typedef double Value;

typedef struct Chunk {
	int codeSize;
	int codeCapacity;
	uint8_t *code;
	int *lines;

	int valuesSize;
	int valuesCapacity;
	Value *values;
} Chunk;

void chunk_initialize(Chunk *chunk);
void chunk_destroy(Chunk *chunk);

void chunk_add_byte(Chunk *chunk, uint8_t byte, int line);
uint16_t chunk_add_constant(Chunk *chunk, Value value);

#endif