#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "scanner.h"

static void _expression(Parser *parser);
static void _number(Parser *parser);
static void _grouping(Parser *parser);
static void _unary(Parser *parser);
static void _binary(Parser *parser);

ParseRule rules[] = {
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

static void _error_current(Parser *parser, char *message) {
	_error_at(parser, &parser->current, message);
}
static void _error_previous(Parser *parser, char *message) {
	_error_at(parser, &parser->previous, message);
}

static void _advance(Parser *parser) {
	parser->previous = parser->current;

	for (;;) {
		parser->current = scanner_next_token(parser->scanner);
		if (parser->current.type != TOKEN_ERROR) break;
		_error_current(parser, parser->current.start);
	}
}

static void _consume(Parser *parser, TokenType type, char *message) {
	if (parser->current.type == type) {
		_advance(parser);
	} else {
		_error_current(parser, message);
	}
}

static void _add_byte(Parser *parser, Byte byte) {
	chunk_add_byte(parser->chunk, byte, parser->previous.line);
}

static void _add_word(Parser *parser, Word word) {
	chunk_add_word(parser->chunk, word, parser->previous.line);
}

static Word _make_constant(Parser *parser, Value value) {
	Word constant = chunk_add_constant(parser->chunk, value);

	if (constant > UINT16_MAX) {
		_error_previous(parser, "Too many constants in one chunk.");
		return 0;
	} else {
		return constant;
	}
}

static ParseRule *_get_rule(TokenType type) {
	return &rules[type];
}

static void _parse_precedence(Parser *parser, Precedence precedence) {
	_advance(parser);

	ParseFn prefixRule = _get_rule(parser->previous.type)->prefix;

	if (prefixRule == NULL) {
		_error_previous(parser, "Expect expression.");
		return;
	}

	prefixRule(parser);

	while (precedence <= _get_rule(parser->current.type)->precedence) {
		_advance(parser);
		ParseFn infixRule = _get_rule(parser->previous.type)->infix;
		infixRule(parser);
	}
}

static void _expression(Parser *parser) {
	_parse_precedence(parser, PRECEDENCE_ASSIGNMENT);
}

static void _number(Parser *parser) {
	double value = strtod(parser->previous.start, NULL);
	_add_byte(parser, OP_CONSTANT);
	_add_word(parser, _make_constant(parser, value));
}

static void _grouping(Parser *parser) {
	_expression(parser);
	_consume(parser, TOKEN_PAREN_RIGHT, "Expect ')' after expression");
}

static void _unary(Parser *parser) {
	TokenType operatorType = parser->previous.type;
	_parse_precedence(parser, PRECEDENCE_UNARY);

	switch (operatorType) {
		case TOKEN_MINUS: _add_byte(parser, OP_NEGATE); break;
		default: break;
	}
}

static void _binary(Parser *parser) {
	TokenType operatorType = parser->previous.type;
	ParseRule *rule = _get_rule(operatorType);
	_parse_precedence(parser, (Precedence)(rule->precedence + 1));

	switch (operatorType) {
		case TOKEN_PLUS: _add_byte(parser, OP_ADD); break;
		case TOKEN_MINUS: _add_byte(parser, OP_SUBTRACT); break;
		case TOKEN_STAR: _add_byte(parser, OP_MULTIPLY); break;
		case TOKEN_SLASH: _add_byte(parser, OP_DIVIDE); break;
		case TOKEN_CARROT: _add_byte(parser, OP_POWER); break;
		default: break;
	}
}

bool compile(Chunk *chunk, char *source) {
	Parser *parser = _create(chunk, source);

	_advance(parser);
	_expression(parser);
	_consume(parser, TOKEN_EOF, "Expect end of expression.");
	_add_byte(parser, OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	if (!parser->error) print_chunk(parser->chunk);
#endif

	bool error = parser->error;
	_destroy(parser);

	return error;
}