#include "debug.h"

static int _constant_instruction(char *name, Chunk *chunk, int offset) {
	printf("%-16s ", name);
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
	switch (value.type) {
		case VALUE_NIL: printf("'nil'"); break;
		case VALUE_BOOL: printf(AS_BOOL(value) ? "'true'" : "'false'"); break;
		case VALUE_NUMBER: printf("'%g'", AS_NUMBER(value)); break;
		default: printf("unknow"); break;
	}
}

int print_instruction(Chunk *chunk, int offset) {
	printf("%04d ", offset);

	Byte instruction = chunk->code[offset];

	switch (instruction) {
		case OP_CONSTANT: return _constant_instruction("OP_CONSTANT", chunk, offset);
		case OP_NIL: return _simple_instruction("OP_NIL", offset);
		case OP_TRUE: return _simple_instruction("OP_TRUE", offset);
		case OP_FALSE: return _simple_instruction("OP_FALSE", offset);
		case OP_EQUAL: return _simple_instruction("OP_EQUAL", offset);
		case OP_GREATER: return _simple_instruction("OP_GREATER", offset);
		case OP_LESS: return _simple_instruction("OP_LESS", offset);
		case OP_NOT: return _simple_instruction("OP_NOT", offset);
		case OP_NEGATE: return _simple_instruction("OP_NEGATE", offset);
		case OP_ADD: return _simple_instruction("OP_ADD", offset);
		case OP_SUBTRACT: return _simple_instruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY: return _simple_instruction("OP_MULTIPLY", offset);
		case OP_DIVIDE: return _simple_instruction("OP_DIVIDE", offset);
		case OP_POWER: return _simple_instruction("OP_POWER", offset);
		case OP_RETURN: return _simple_instruction("OP_RETURN", offset);
		default: return _unknow_instruction(instruction, offset);
	}
}

void print_chunk(Chunk *chunk) {
	for (int offset = 0; offset < chunk->codeSize;) offset = print_instruction(chunk, offset);
}

void print_stack(VM *vm) {
	printf("{");
	for (int i = 0; i < vm->stackSize; i++) {
		print_value(vm->stack[i]);
		if (i != vm->stackSize - 1) printf(", ");
	}
	printf("}\n");
}