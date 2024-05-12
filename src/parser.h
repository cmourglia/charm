#pragma once

#include "token.h"

struct Lexer;
struct Expr;

typedef struct Parser {
	struct Lexer *lexer;

	Token curr_token;
	Token prev_token;
} Parser;

Parser parser_init(struct Lexer *lexer);

struct Expr *parser_parse_program(Parser *parser);
