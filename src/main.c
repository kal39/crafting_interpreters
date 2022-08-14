#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static void _repl(VM *vm) {
	char line[1024];
	for (;;) {
		printf(" > ");

		if (!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}

		vm_interpret(vm, line);
	}
}

static char *_read_file(char *path) {
	FILE *fp = fopen(path, "rb");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file \"%s\"\n", path);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);

	char *buff = (char *)malloc(fileSize + 1);
	fread(buff, sizeof(char), fileSize, fp);
	buff[fileSize] = '\0';

	fclose(fp);
	return buff;
}

static void _run_file(VM *vm, char *path) {
	char *source = _read_file(path);
	InterpretResult result = vm_interpret(vm, source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR) {
		fprintf(stderr, "Compilation error\n");
		exit(1);
	}

	if (result == INTERPRET_RUNTIME_ERROR) {
		fprintf(stderr, "Runtime error\n");
		exit(1);
	}
}

int main(int argc, char *argv[]) {
	VM *vm = vm_create();

	if (argc == 1) _repl(vm);
	else if (argc == 2) _run_file(vm, argv[1]);
	else {
		fprintf(stderr, "Usage: k_Lang [path]\n");
		exit(1);
	}

	vm_destroy(vm);

	return 0;
}