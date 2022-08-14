#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"
#include "common.h"
#include "vm.h"

void print_value(Value value);
int print_instruction(Chunk *chunk, int offset);
void print_chunk(Chunk *chunk);
void print_stack(VM *vm);

#endif