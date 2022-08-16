#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

#define PEEK(distance) vm_peek(vm)
#define POP() vm_pop(vm)
#define PUSH(value) vm_push(vm, (value))

#define POP_NUMBER() (_pop_number(vm))
#define PUSH_NUMBER(value) PUSH(MAKE_NUMBER(value))

#define PUSH_BOOL(value) PUSH(MAKE_BOOL(value));
#define PUSH_NIL() PUSH(MAKE_NIL())

#define READ_BYTE() (*vm->ip++)
#define READ_WORD() (*(Word *)((vm->ip += 2) - 2))

static void _clear_stack(VM *vm) {
	vm->ip = NULL;
	vm->stackSize = 0;
}

static void _runtime_error(VM *vm, const char *format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm->ip - vm->chunk->code - 1;
	int line = vm->chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);

	_clear_stack(vm);
}

static double _pop_number(VM *vm) {
	if (!IS_NUMBER(PEEK(0))) {
		_runtime_error(vm, "Must be a number");
		return NAN;
	} else {
		return AS_NUMBER(POP());
	}
}

static InterpretResult _run(VM *vm) {
	for (;;) {
		if (vm->ip == NULL) return INTERPRET_RUNTIME_ERROR;

#ifdef DEBUG_TRACE_EXECUTION
		printf("\n");
		print_stack(vm);
		print_instruction(vm->chunk, vm->ip - vm->chunk->code);
		printf("\n");
#endif

		Byte instruction;
		switch (instruction = READ_BYTE()) {
			case OP_CONSTANT: PUSH(vm->chunk->values[READ_WORD()]); break;
			case OP_NIL: PUSH_NIL(); break;
			case OP_TRUE: PUSH_BOOL(true); break;
			case OP_FALSE: PUSH_BOOL(false); break;
			case OP_EQUAL: {
				Value b = POP();
				Value a = POP();
				if (a.type != b.type) {
					PUSH_BOOL(false);
					break;
				}
				switch (a.type) {
					case VALUE_BOOL: PUSH_BOOL(AS_BOOL(a) == AS_BOOL(b)); break;
					case VALUE_NIL: PUSH_BOOL(true); break;
					case VALUE_NUMBER: PUSH_BOOL(AS_NUMBER(a) == AS_NUMBER(b));
				}
				break;
			}
			case OP_GREATER: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_BOOL(a > b);
				break;
			}
			case OP_LESS: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_BOOL(a < b);
				break;
			}
			case OP_NOT: {
				Value a = POP();
				switch (a.type) {
					case VALUE_BOOL: PUSH_BOOL(!AS_BOOL(a)); break;
					case VALUE_NIL: PUSH_BOOL(true); break;
					default: PUSH_BOOL(false); break;
				}
				break;
			}
			case OP_NEGATE: PUSH_NUMBER(-POP_NUMBER()); break;
			case OP_ADD: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_NUMBER(a + b);
				break;
			}
			case OP_SUBTRACT: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_NUMBER(a - b);
				break;
			}
			case OP_MULTIPLY: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_NUMBER(a * b);
				break;
			}
			case OP_DIVIDE: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_NUMBER(a / b);
				break;
			}
			case OP_POWER: {
				double b = POP_NUMBER();
				double a = POP_NUMBER();
				PUSH_NUMBER(pow(a, b));
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
	_clear_stack(vm);
	return vm;
}

void vm_destroy(VM *vm) {
	vm = FREE_STRUCT(VM, vm);
}

void vm_push(VM *vm, Value value) {
	vm->stack[vm->stackSize++] = value;
}

Value vm_peek(VM *vm) {
	return vm->stack[vm->stackSize - 1];
}

Value vm_pop(VM *vm) {
	return vm->stack[--(vm->stackSize)];
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