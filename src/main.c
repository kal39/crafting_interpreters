#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char *argv[]) {
	Chunk *chunk = chunk_create();
	VM *vm = vm_create();

	uint16_t constant = chunk_add_constant(chunk, 2);
	chunk_add_byte(chunk, OP_CONSTANT, 123);
	chunk_add_word(chunk, constant, 123);

	constant = chunk_add_constant(chunk, 3);
	chunk_add_byte(chunk, OP_CONSTANT, 123);
	chunk_add_word(chunk, constant, 123);

	chunk_add_byte(chunk, OP_MULTIPLY, 123);

	constant = chunk_add_constant(chunk, 4);
	chunk_add_byte(chunk, OP_CONSTANT, 123);
	chunk_add_word(chunk, constant, 123);

	chunk_add_byte(chunk, OP_ADD, 123);

	chunk_add_byte(chunk, OP_RETURN, 123);

	printf("\n=== PRINTING CHUNK ===\n\n");
	print_chunk(chunk);

	printf("\n=== INTERPRETING CHUNK ===\n\n");
	vm_interpret(vm, chunk);

	vm_destroy(vm);
	chunk_destroy(chunk);

	return 0;
}