#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"

typedef enum OpCode {
	OP_CONSTANT,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NEGATE,
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

Chunk *chunk_create();
void chunk_destroy(Chunk *chunk);

void chunk_add_byte(Chunk *chunk, uint8_t byte, int line);
void chunk_add_word(Chunk *chunk, uint16_t word, int line);
uint16_t chunk_add_constant(Chunk *chunk, Value value);

#endif