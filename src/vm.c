#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

#define POP() vm_pop(vm);
#define PUSH(value) vm_push(vm, (value))

static Byte _read_byte(VM *vm) {
	Byte byte = *vm->ip;
	vm->ip++;
	return byte;
}

static Word _read_word(VM *vm) {
	Word word = *(Word *)vm->ip;
	vm->ip += 2;
	return word;
}

static InterpretResult _run(VM *vm) {
	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("\n");
		print_stack(vm);
		print_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif

		Byte instruction;
		switch (instruction = _read_byte(vm)) {
			case OP_CONSTANT: {
				PUSH(vm->chunk->values[_read_word(vm)]);
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

			case OP_RETURN: {
				printf(" = ");
				double a = POP();
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

void vm_push(VM *vm, Value value) {
	*vm->stackTop = value;
	vm->stackTop++;
}

Value vm_pop(VM *vm) {
	vm->stackTop--;
	return *vm->stackTop;
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