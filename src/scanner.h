#ifndef SCANNER_H
#define SCANNER_H

#include "common.h"

typedef enum TokenType {
	// Single char
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_COMMA,
	TOKEN_DOT,
	TOKEN_MINUS,
	TOKEN_PLUS,
	TOKEN_SEMICOLON,
	TOKEN_SLASH,
	TOKEN_STAR,

	// Single or double
	TOKEN_BANG,
	TOKEN_BANG_EQUAL,
	TOKEN_EQUAL,
	TOKEN_EQUAL_EQUAL,
	TOKEN_GREATER,
	TOKEN_GREATER_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL,

	// Literals
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_NUMBER,

	// Keywords
	TOKEN_AND,
	TOKEN_CLASS,
	TOKEN_ELSE,
	TOKEN_FALSE,
	TOKEN_FOR,
	TOKEN_FUN,
	TOKEN_IF,
	TOKEN_NIL,
	TOKEN_OR,
	TOKEN_PRINT,
	TOKEN_RETURN,
	TOKEN_SUPER,
	TOKEN_THIS,
	TOKEN_TRUE,
	TOKEN_VAR,
	TOKEN_WHILE,

	// Other
	TOKEN_ERROR,
	TOKEN_EOF
} TokenType;

typedef struct Token {
	TokenType type;
	char *start;
	int length;
	int line;
} Token;

typedef struct Scanner {
	char *start;
	char *current;
	int line;
} Scanner;

Scanner *scanner_create(char *source);
void scanner_destroy(Scanner *scanner);

Token scanner_next_token(Scanner *scanner);

#endif