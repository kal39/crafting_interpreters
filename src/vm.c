#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

#define POP() VM_POP(vm)
#define PUSH(value) VM_PUSH(vm, (value))
#define READ_BYTE() (*vm->ip++)
#define READ_WORD() (*(Word *)((vm->ip += 2) - 2))

static InterpretResult _run(VM *vm) {
	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("\n");
		print_stack(vm);
		print_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif

		Byte instruction;
		switch (instruction = READ_BYTE()) {
			case OP_CONSTANT: {
				PUSH(vm->chunk->values[READ_WORD()]);
				break;
			}
			case OP_NEGATE: {
				double a = POP();
				PUSH(-a);
				break;
			}
			case OP_ADD: {
				double b = POP();
				double a = POP();
				PUSH(a + b);
				break;
			}
			case OP_SUBTRACT: {
				double b = POP();
				double a = POP();
				PUSH(a - b);
				break;
			}
			case OP_MULTIPLY: {
				double b = POP();
				double a = POP();
				PUSH(a * b);
				break;
			}
			case OP_DIVIDE: {
				double b = POP();
				double a = POP();
				PUSH(a / b);
				break;
			}
			case OP_POWER: {
				double b = POP();
				double a = POP();
				PUSH(pow(a, b));
				break;
			}
			case OP_RETURN: {
				Value a = POP();
				printf(" = ");
				print_value(a);
				printf("\n");
				return INTERPRET_OK;
			}
		}
	}
}

VM *vm_create() {
	VM *vm = ALLOCATE_STRUCT(VM);
	vm->ip = NULL;
	vm->stackTop = vm->stack;
	return vm;
}

void vm_destroy(VM *vm) {
	vm = FREE_STRUCT(VM, vm);
}

InterpretResult vm_interpret(VM *vm, char *source) {
	Chunk *chunk = chunk_create();

	if (compile(chunk, source)) return INTERPRET_COMPILE_ERROR;

	vm->chunk = chunk;
	vm->ip = chunk->code;

	InterpretResult result = _run(vm);

	chunk_destroy(chunk);

	return result;
}