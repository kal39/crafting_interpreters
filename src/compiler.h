#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"
#include "scanner.h"

typedef struct Parser {
	Chunk *chunk;
	Scanner *scanner;

	Token current;
	Token previous;

	bool error;
	bool panic;
} Parser;

bool compile(Chunk *chunk, char *source);

#endif