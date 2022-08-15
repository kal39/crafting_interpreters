#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "common.h"

typedef struct VM {
	Chunk *chunk;
	Byte *ip;

	Value *stackTop;
	Value stack[STACK_SIZE];
} VM;

typedef enum InterpretResult {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

VM *vm_create();
void vm_destroy(VM *vm);

void vm_push(VM *vm, Value value);
Value vm_pop(VM *vm);

InterpretResult vm_interpret(VM *vm, char *source);

#endif