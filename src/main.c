#include <stdio.h>

#include "core/memory.h"
#include "core/value.h"

#include "ast/parser.h"
#include "ast/lexer.h"

#include "compiler/chunk.h"
#include "compiler/compiler.h"

#include "debug/debug.h"

#include "interpreter/treewalk.h"
#include "interpreter/vm.h"

static void usage(int argc, char **argv);
static char *read_whole_file(const char *filename);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		usage(argc, argv);
		return 1;
	}

	const char *src = read_whole_file(argv[1]);

	if (src == NULL)
	{
		return 2;
	}

	Lexer lexer = lexer_init(src);
	Parser parser = parser_init(&lexer);

	Program program = parser_parse_program(&parser);

	printf("-*-*-*- AST -*-*-*-\n");
	debug_print_program(program);

	printf("\n-*-*-*- Treewalk Interpret -*-*-*-\n");
	treewalk_interpreter_run(program);

	printf("\n\n");

	Chunk chunk;
	chunk_init(&chunk);

	Compiler compiler;
	compiler_init(&compiler, &chunk);

	compile_program(&compiler, program);
	printf("\n-*-*-*- Compiled Bytecode -*-*-*-\n");
	debug_disassemble_chunk(&chunk, "test chunk");

	printf("\n-*-*-*- Running program -*-*-*-\n");
	vm_init();
	vm_interpret(&chunk);

	vm_free();

	chunk_free(&chunk);
}

static void usage(int argc, char **argv)
{
	printf("Usage: %s <filename.charm>\n", argv[0]);
}

static char *read_whole_file(const char *filename)
{
	FILE *file = fopen(filename, "rb");

	if (file == NULL)
	{
		printf("Could not find file '%s'\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	u64 size = ftell(file);
	rewind(file);

	char *content = (char *)mem_realloc(NULL, size + 1);
	if (content == NULL)
	{
		printf("Could not alloc %zu bytes to read file\n", size + 1);
		fclose(file);
		return NULL;
	}

	int read = fread(content, size, 1, file);

	if (read != 1)
	{
		printf("Something went wrong while reading file\n");
		fclose(file);
		mem_free(content);
		return NULL;
	}

	return content;
}
