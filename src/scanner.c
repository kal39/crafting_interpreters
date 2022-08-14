#include "scanner.h"
#include "memory.h"

static bool _is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool _is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool _scanner_at_end(Scanner *scanner) {
	return *scanner->current == '\0';
}

static Token _scanner_make_token(Scanner *scanner, TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner->start;
	token.length = (int)(scanner->current - scanner->start);
	token.line = scanner->line;
	return token;
}

static Token _scanner_make_error_token(Scanner *scanner, char *message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner->line;
	return token;
}

static char _scanner_advance(Scanner *scanner) {
	scanner->current++;
	return scanner->current[-1];
}

static char _scanner_peek(Scanner *scanner) {
	return scanner->current[0];
}

static char _scanner_peek_next(Scanner *scanner) {
	if (_scanner_at_end(scanner)) return '\0';
	return scanner->current[1];
}

static bool _scanner_match(Scanner *scanner, char c) {
	if (_scanner_at_end(scanner) || *scanner->current != c) {
		return false;
	} else {
		scanner->current++;
		return true;
	}
}

static void _scanner_skip_whitespace(Scanner *scanner) {
	for (;;) {
		char c = _scanner_peek(scanner);

		if (c == ' ' || c == '\r' || c == '\t') {
			_scanner_advance(scanner);
		} else if (c == '\n') {
			scanner->line++;
			_scanner_advance(scanner);
		} else if (c == '/') {
			if (_scanner_peek_next(scanner) == '/') {
				while (_scanner_peek(scanner) != '\n' && !_scanner_at_end(scanner)) _scanner_advance(scanner);
			}
		} else {
			break;
		}
	}
}

static bool _scanner_check_word(Scanner *scanner, char *word) {
	int len = strlen(word);
	return scanner->current - scanner->start == len && memcmp(scanner->start, word, len) == 0;
}

static TokenType _scanner_get_identifier_type(Scanner *scanner) {
	if (_scanner_check_word(scanner, "and")) return TOKEN_AND;
	if (_scanner_check_word(scanner, "class")) return TOKEN_CLASS;
	if (_scanner_check_word(scanner, "else")) return TOKEN_ELSE;
	if (_scanner_check_word(scanner, "false")) return TOKEN_FALSE;
	if (_scanner_check_word(scanner, "for")) return TOKEN_FOR;
	if (_scanner_check_word(scanner, "fun")) return TOKEN_FUN;
	if (_scanner_check_word(scanner, "if")) return TOKEN_IF;
	if (_scanner_check_word(scanner, "nil")) return TOKEN_NIL;
	if (_scanner_check_word(scanner, "or")) return TOKEN_OR;
	if (_scanner_check_word(scanner, "print")) return TOKEN_PRINT;
	if (_scanner_check_word(scanner, "return")) return TOKEN_RETURN;
	if (_scanner_check_word(scanner, "super")) return TOKEN_SUPER;
	if (_scanner_check_word(scanner, "this")) return TOKEN_THIS;
	if (_scanner_check_word(scanner, "true")) return TOKEN_TRUE;
	if (_scanner_check_word(scanner, "var")) return TOKEN_VAR;
	if (_scanner_check_word(scanner, "while")) return TOKEN_WHILE;

	return TOKEN_IDENTIFIER;
}

static Token _scanner_make_string_token(Scanner *scanner) {
	while (_scanner_peek(scanner) != '"' && !_scanner_at_end(scanner)) {
		if (_scanner_peek(scanner) == '\n') scanner->line++;
		_scanner_advance(scanner);
	}

	if (_scanner_at_end(scanner)) return _scanner_make_error_token(scanner, "Unterminated string");

	_scanner_advance(scanner);
	return _scanner_make_token(scanner, TOKEN_STRING);
}

static Token _scanner_make_number_token(Scanner *scanner) {
	while (_is_digit(_scanner_peek(scanner))) _scanner_advance(scanner);

	if (_scanner_peek(scanner) == '.' && _is_digit(_scanner_peek_next(scanner))) {
		_scanner_advance(scanner);
		while (_is_digit(_scanner_peek(scanner))) _scanner_advance(scanner);
	}

	return _scanner_make_token(scanner, TOKEN_NUMBER);
}

static Token _scanner_make_identifier_token(Scanner *scanner) {
	while (_is_alpha(_scanner_peek(scanner)) || _is_digit(_scanner_peek(scanner))) _scanner_advance(scanner);
	return _scanner_make_token(scanner, _scanner_get_identifier_type(scanner));
}

Scanner *scanner_create(char *source) {
	Scanner *scanner = ALLOCATE_STRUCT(Scanner);

	scanner->start = source;
	scanner->current = source;
	scanner->line = 1;

	return scanner;
}

void scanner_destroy(Scanner *scanner) {
	FREE_STRUCT(Scanner, scanner);
}

Token scanner_next_token(Scanner *scanner) {
	_scanner_skip_whitespace(scanner);
	scanner->start = scanner->current;

	if (_scanner_at_end(scanner)) return _scanner_make_token(scanner, TOKEN_EOF);

	char c = _scanner_advance(scanner);

	if (_is_alpha(c)) return _scanner_make_identifier_token(scanner);
	if (_is_digit(c)) return _scanner_make_number_token(scanner);

	switch (c) {
		case '(': return _scanner_make_token(scanner, TOKEN_LEFT_PAREN);
		case ')': return _scanner_make_token(scanner, TOKEN_RIGHT_PAREN);
		case '{': return _scanner_make_token(scanner, TOKEN_LEFT_BRACE);
		case '}': return _scanner_make_token(scanner, TOKEN_RIGHT_BRACE);
		case ';': return _scanner_make_token(scanner, TOKEN_SEMICOLON);
		case ',': return _scanner_make_token(scanner, TOKEN_COMMA);
		case '.': return _scanner_make_token(scanner, TOKEN_DOT);
		case '-': return _scanner_make_token(scanner, TOKEN_MINUS);
		case '+': return _scanner_make_token(scanner, TOKEN_PLUS);
		case '/': return _scanner_make_token(scanner, TOKEN_SLASH);
		case '*': return _scanner_make_token(scanner, TOKEN_STAR);
		case '!': return _scanner_make_token(scanner, _scanner_match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=': return _scanner_make_token(scanner, _scanner_match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<': return _scanner_make_token(scanner, _scanner_match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>': return _scanner_make_token(scanner, _scanner_match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"': return _scanner_make_string_token(scanner);
	}

	return _scanner_make_error_token(scanner, "Unexpected character");
}