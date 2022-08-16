#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"

typedef enum {
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_NOT,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_POWER,
	OP_NEGATE,
	OP_RETURN,
} OpCode;

typedef enum {
	VALUE_NIL,
	VALUE_BOOL,
	VALUE_NUMBER,
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;
} Value;

typedef struct {
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

#define IS_NIL(value) ((value).type == VALUE_NIL)
#define IS_BOOL(value) ((value).type == VALUE_BOOL)
#define IS_NUMBER(value) ((value).type == VALUE_NUMBER)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

#define MAKE_NIL() ((Value){.type = VALUE_NIL, .as = {.number = 0}})
#define MAKE_BOOL(value) ((Value){.type = VALUE_BOOL, .as = {.boolean = (value)}})
#define MAKE_NUMBER(value) ((Value){.type = VALUE_NUMBER, .as = {.number = (value)}})

#endif