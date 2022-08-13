#include "chunk.h"
#include "chunk_print.h"
#include "common.h"

int main(int argc, char *argv[]) {
	Chunk chunk;
	chunk_initialize(&chunk);

	for (int i = 0; i < 500; i++) {
		uint16_t constant = chunk_add_constant(&chunk, i * 0.0001);
		chunk_add_byte(&chunk, OP_CONSTANT, 123);
		chunk_add_byte(&chunk, (constant >> 8) & 0xFF, 123);
		chunk_add_byte(&chunk, (constant >> 0) & 0xFF, 123);
	}

	chunk_add_byte(&chunk, OP_RETURN, 123);

	chunk_print(&chunk);

	chunk_destroy(&chunk);

	return 0;
}