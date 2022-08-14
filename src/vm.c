#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

#define POP() vm_pop(vm);
#define PUSH(value) vm_push(vm, (value))

static uint8_t _vm_read_byte(VM *vm) {
	uint8_t byte = *vm->ip;
	vm->ip++;
	return byte;
}

static uint16_t _vm_read_word(VM *vm) {
	uint16_t word = *(uint16_t *)vm->ip;
	vm->ip += 2;
	return word;
}

static InterpretResult _vm_run(VM *vm) {
	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("\n");
		print_stack(vm);
		print_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif

		uint8_t instruction;
		switch (instruction = _vm_read_byte(vm)) {
			case OP_CONSTANT: {
				PUSH(vm->chunk->values[_vm_read_word(vm)]);
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
				printf(" > ");
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
	compile(source);
	return _vm_run(vm);
}