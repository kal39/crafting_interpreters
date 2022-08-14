#include <stdio.h>

#include "compiler.h"
#include "scanner.h"

void compile(char *source) {
	Scanner *scanner = scanner_create(source);

	for (int line = -1;;) {
		Token token = scanner_next_token(scanner);
		if (token.line != line) {
			printf("%4d ", token.line);
			line = token.line;
		} else {
			printf("     ");
		}
		printf("%2d '%.*s'\n", token.type, token.length, token.start);

		if (token.type == TOKEN_EOF) break;
	}

	scanner_destroy(scanner);
}