#include "parser.h"
#include "lexer.h"
#include "ast.h"

#include "debug.h"
#include "interpreter.h"

int main(void)
{
	//const char *str = "-123 * (45.67);\nprint ((234 + 678) / 22) == 33;";
	const char *str = ""
					  "var toto = 2 * 3 + 4;\n"
					  "var titi;\n"
					  "print toto;\n"
					  "print titi;\n";

	Lexer lexer = lexer_init(str);
	Parser parser = parser_init(&lexer);

	Program program = parser_parse_program(&parser);

	debug_print_program(program);

	interpreter_run(program);

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
