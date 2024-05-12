#include "token.h"
#include <stdio.h>

#include "lexer.h"

const char *str = "function foo(a, b) {\n"
				  "  return a + b;\n"
				  "}\n"
				  "\n"
				  "// Comment\n"
				  "if (a >= b or a < b or a == b) {\n"
				  "  b = a;\n"
				  "}\n"
				  "a = 3.1415\n"
				  "b = 42\n"
				  "c = \"This is a string\"\n"
				  "d = 4.";

int main(void)
{
	Lexer lexer = lexer_init(str);

	for (;;)
	{
		Token token = lexer_get_next_token(&lexer);

		char buf[512];
		int sz = token_to_string(buf, 512, token);
		printf("%s\n", buf);

		if (token.type == Token_EOF)
		{
			break;
		}
	}
}
