#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "scanner.h"

#define WRITE_BYTE(byte) chunk_add_byte(parser->chunk, (byte), parser->previous.line)
#define WRITE_WORD(word) chunk_add_word(parser->chunk, (word), parser->previous.line);

#define ERROR_CURENT(message) _error_at(parser, &parser->current, (message))
#define ERROR_PREVIOUS(message) _error_at(parser, &parser->previous, (message))

#define GET_RULE(type) (&RULES[(type)])

typedef enum _Precedence {
	PRECEDENCE_NONE,
	PRECEDENCE_ASSIGNMENT,
	PRECEDENCE_OR,
	PRECEDENCE_AND,
	PRECEDENCE_EQUALITY,
	PRECEDENCE_COMPARISON,
	PRECEDENCE_TERM,
	PRECEDENCE_FACTOR,
	PRECEDENCE_POWER,
	PRECEDENCE_UNARY,
	PRECEDENCE_CALL,
	PRECEDENCE_PRIMARY
} _Precedence;

typedef void (*_ParseFn)(Parser *);

typedef struct _ParseRule {
	_ParseFn prefix;
	_ParseFn infix;
	_Precedence precedence;
} _ParseRule;

static void _expression(Parser *parser);
static void _number(Parser *parser);
static void _grouping(Parser *parser);
static void _unary(Parser *parser);
static void _binary(Parser *parser);

static _ParseRule RULES[] = {
	[TOKEN_PAREN_LEFT] = {_grouping, NULL, PRECEDENCE_NONE},
	[TOKEN_PAREN_RIGHT] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_BRACE_LEFT] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_BRACE_RIGHT] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_COMMA] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_DOT] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_MINUS] = {_unary, _binary, PRECEDENCE_TERM},
	[TOKEN_PLUS] = {NULL, _binary, PRECEDENCE_TERM},
	[TOKEN_SEMICOLON] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_SLASH] = {NULL, _binary, PRECEDENCE_FACTOR},
	[TOKEN_STAR] = {NULL, _binary, PRECEDENCE_FACTOR},
	[TOKEN_CARROT] = {NULL, _binary, PRECEDENCE_POWER},
	[TOKEN_BANG] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_BANG_EQUAL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_EQUAL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_EQUAL_EQUAL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_GREATER] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_GREATER_EQUAL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_LESS] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_LESS_EQUAL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_IDENTIFIER] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_STRING] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_NUMBER] = {_number, NULL, PRECEDENCE_NONE},
	[TOKEN_AND] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_CLASS] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_ELSE] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_FALSE] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_FOR] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_FUN] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_IF] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_NIL] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_OR] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_PRINT] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_RETURN] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_SUPER] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_THIS] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_TRUE] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_VAR] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_WHILE] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_ERROR] = {NULL, NULL, PRECEDENCE_NONE},
	[TOKEN_EOF] = {NULL, NULL, PRECEDENCE_NONE},
};

static Parser *_create(Chunk *chunk, char *source) {
	Parser *parser = ALLOCATE_STRUCT(Parser);

	parser->chunk = chunk;
	parser->scanner = scanner_create(source);
	parser->error = false;
	parser->panic = false;

	return parser;
}

static void _destroy(Parser *parser) {
	scanner_destroy(parser->scanner);
	FREE_STRUCT(Parser, parser);
}

static void _error_at(Parser *parser, Token *token, char *message) {
	if (parser->panic) return;
	parser->panic = true;

	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	} else if (token->type == TOKEN_ERROR) {
		// TODO:
	} else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	fprintf(stderr, ": %s\n", message);
	parser->error = true;
}

static void _advance(Parser *parser) {
	parser->previous = parser->current;

	for (;;) {
		parser->current = scanner_next_token(parser->scanner);
		if (parser->current.type != TOKEN_ERROR) break;
		ERROR_CURENT(parser->current.start);
	}
}

static void _consume(Parser *parser, TokenType type, char *message) {
	if (parser->current.type == type) {
		_advance(parser);
	} else {
		ERROR_CURENT(message);
	}
}

static Word _make_constant(Parser *parser, Value value) {
	Word constant = chunk_add_constant(parser->chunk, value);

	if (constant > UINT16_MAX) {
		ERROR_PREVIOUS("Too many constants in one chunk");
		return 0;
	} else {
		return constant;
	}
}

static void _parse_precedence(Parser *parser, _Precedence precedence) {
	_advance(parser);

	_ParseFn prefixRule = GET_RULE(parser->previous.type)->prefix;

	if (prefixRule == NULL) {
		ERROR_PREVIOUS("Expect expression.");
		return;
	}

	prefixRule(parser);

	while (precedence <= GET_RULE(parser->current.type)->precedence) {
		_advance(parser);
		_ParseFn infixRule = GET_RULE(parser->previous.type)->infix;
		infixRule(parser);
	}
}

static void _expression(Parser *parser) {
	_parse_precedence(parser, PRECEDENCE_ASSIGNMENT);
}

static void _binary(Parser *parser) {
	TokenType operatorType = parser->previous.type;
	_ParseRule *rule = GET_RULE(operatorType);
	_parse_precedence(parser, (_Precedence)(rule->precedence + 1));

	switch (operatorType) {
		case TOKEN_PLUS: WRITE_BYTE(OP_ADD); break;
		case TOKEN_MINUS: WRITE_BYTE(OP_SUBTRACT); break;
		case TOKEN_STAR: WRITE_BYTE(OP_MULTIPLY); break;
		case TOKEN_SLASH: WRITE_BYTE(OP_DIVIDE); break;
		case TOKEN_CARROT: WRITE_BYTE(OP_POWER); break;
		default: break;
	}
}

static void _unary(Parser *parser) {
	TokenType operatorType = parser->previous.type;
	_parse_precedence(parser, PRECEDENCE_UNARY);

	switch (operatorType) {
		case TOKEN_MINUS: WRITE_BYTE(OP_NEGATE); break;
		default: break;
	}
}

static void _grouping(Parser *parser) {
	_expression(parser);
	_consume(parser, TOKEN_PAREN_RIGHT, "Expect ')'");
}

static void _number(Parser *parser) {
	double value = strtod(parser->previous.start, NULL);
	WRITE_BYTE(OP_CONSTANT);
	WRITE_WORD(_make_constant(parser, value));
}

bool compile(Chunk *chunk, char *source) {
	Parser *parser = _create(chunk, source);

	_advance(parser);
	_expression(parser);
	_consume(parser, TOKEN_EOF, "Expect end of expression");
	WRITE_BYTE(OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	if (!parser->error) print_chunk(parser->chunk);
#endif

	bool error = parser->error;
	_destroy(parser);

	return error;
}