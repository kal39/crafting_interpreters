#include "debug.h"

static int _constant_instruction(char *name, Chunk *chunk, int offset) {
	printf("%-16s %03d %03d ", name, chunk->code[offset + 1], chunk->code[offset + 2]);
	print_value(chunk->values[*(Word *)&chunk->code[offset + 1]]);
	printf("\n");
	return offset + 3;
}

static int _simple_instruction(char *name, int offset) {
	printf("%-16s\n", name);
	return offset + 1;
}

static int _unknow_instruction(Byte instruction, int offset) {
	printf("Unknown opcode %04d\n", instruction);
	return offset + 1;
}

void print_value(Value value) {
	printf("'%g'", value);
}

int print_instruction(Chunk *chunk, int offset) {
	printf("%04d ", offset);

	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) printf("     ");
	else printf("%4d ", chunk->lines[offset]);

	Byte instruction = chunk->code[offset];

	switch (instruction) {
		case OP_CONSTANT: return _constant_instruction("OP_CONSTANT", chunk, offset);
		case OP_ADD: return _simple_instruction("OP_ADD", offset);
		case OP_SUBTRACT: return _simple_instruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY: return _simple_instruction("OP_MULTIPLY", offset);
		case OP_DIVIDE: return _simple_instruction("OP_DIVIDE", offset);
		case OP_NEGATE: return _simple_instruction("OP_NEGATE", offset);
		case OP_RETURN: return _simple_instruction("OP_RETURN", offset);
		default: return _unknow_instruction(instruction, offset);
	}
}

void print_chunk(Chunk *chunk) {
	for (int offset = 0; offset < chunk->codeSize;) offset = print_instruction(chunk, offset);
}

void print_stack(VM *vm) {
	printf("{ ");
	for (Value *ptr = vm->stack; ptr < vm->stackTop; ptr++) {
		print_value(*ptr);
		printf(" ");
	}
	printf("}\n");
}