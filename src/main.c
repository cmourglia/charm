#include "parser.h"
#include "lexer.h"
#include "ast.h"

#include "debug.h"

int main(void)
{
	const char *str = "-123 * (45.67)";

	Lexer lexer = lexer_init(str);
	Parser parser = parser_init(&lexer);

	struct Expr *program = parser_parse_program(&parser);

	debug_print_expr(program);

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
