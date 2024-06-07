#include <stdio.h>

#include "core/memory.h"
#include "core/value.h"

#include "ast/parser.h"
#include "ast/lexer.h"

#include "compiler/chunk.h"

#include "debug/debug.h"

#include "interpreter/treewalk.h"

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

	debug_print_program(program);

	treewalk_interpreter_run(program);

	printf("\n\n");

	Chunk chunk;
	chunk_init(&chunk);

	chunk_write_constant(&chunk, value_number(42));

	chunk_write(&chunk, Op_Return);
	debug_disassemble_chunk(&chunk, "test chunk");

	chunk_free(&chunk);

	//for (;;)
	//{
	//	Token token = lexer_get_next_token(&lexer);

	//	char buf[512];
	//	int sz = token_to_string(buf, 512, token);
	//	UNUSED(sz);

	//	printf("%s\n", buf);

	//	if (token.type == Token_EOF)
	//	{
	//		break;
	//	}
	//}
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
