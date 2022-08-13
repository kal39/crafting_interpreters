#include <stdio.h>

#include "chunk_print.h"

static void _print_value(Value value) {
	printf("%g", value);
}

static int _constant_instruction(const char *name, Chunk *chunk, int offset) {
	printf("%-16s %03d %03d \"", name, chunk->code[offset + 1], chunk->code[offset + 2]);
	_print_value(chunk->values[(chunk->code[offset + 1] << 8) | (chunk->code[offset + 2])]);
	printf("\"\n");
	return offset + 3;
}

static int _simple_instruction(const char *name, int offset) {
	printf("%-16s\n", name);
	return offset + 1;
}

static int _unknow_instruction(uint8_t instruction, int offset) {
	printf("Unknown opcode %04d\n", instruction);
	return offset + 1;
}

static int _print_instruction(Chunk *chunk, int offset) {
	printf("%04d ", offset);

	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) printf("     ");
	else printf("%4d ", chunk->lines[offset]);

	uint8_t instruction = chunk->code[offset];

	switch (instruction) {
		case OP_CONSTANT: return _constant_instruction("OP_CONSTANT", chunk, offset);
		case OP_RETURN: return _simple_instruction("OP_RETURN", offset);
		default: return _unknow_instruction(instruction, offset);
	}
}

void chunk_print(Chunk *chunk) {
	for (int offset = 0; offset < chunk->codeSize;) offset = _print_instruction(chunk, offset);
}