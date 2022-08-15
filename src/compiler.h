#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"
#include "scanner.h"

typedef enum Precedence {
	PRECEDENCE_NONE,
	PRECEDENCE_ASSIGNMENT,
	PRECEDENCE_OR,
	PRECEDENCE_AND,
	PRECEDENCE_EQUALITY,
	PRECEDENCE_COMPARISON,
	PRECEDENCE_TERM,
	PRECEDENCE_FACTOR,
	PRECEDENCE_UNARY,
	PRECEDENCE_CALL,
	PRECEDENCE_PRIMARY
} Precedence;

typedef struct Parser {
	Chunk *chunk;
	Scanner *scanner;

	Token current;
	Token previous;

	bool error;
	bool panic;
} Parser;

typedef void (*ParseFn)(Parser *);

typedef struct ParseRule {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

bool compile(Chunk *chunk, char *source);

#endif