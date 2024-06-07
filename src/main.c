#include "ast/parser.h"
#include "ast/lexer.h"

#include "debug/ast_printer.h"
#include "interpreter/treewalk.h"

#include "core/beard_lib.h"

static void usage(int argc, char **argv);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		usage(argc, argv);
		return 1;
	}

	const char *src = beard_read_whole_file(argv[1]);

	if (src == NULL)
	{
		return 2;
	}

	Lexer lexer = lexer_init(src);
	Parser parser = parser_init(&lexer);

	Program program = parser_parse_program(&parser);

	debug_print_program(program);

	treewalk_interpreter_run(program);

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
