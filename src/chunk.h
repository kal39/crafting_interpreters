#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"

typedef enum OpCode {
	OP_CONSTANT,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_POWER,
	OP_NEGATE,
	OP_RETURN,
} OpCode;

typedef double Value;

typedef struct Chunk {
	int codeSize;
	int codeCapacity;
	Byte *code;
	int *lines;

	int valuesSize;
	int valuesCapacity;
	Value *values;
} Chunk;

Chunk *chunk_create();
void chunk_destroy(Chunk *chunk);

void chunk_add_byte(Chunk *chunk, Byte byte, int line);
void chunk_add_word(Chunk *chunk, Word word, int line);
Word chunk_add_constant(Chunk *chunk, Value value);

#endif