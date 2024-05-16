#pragma once

#include "token.h"

typedef struct Parser {
	struct Lexer *lexer;

	Token curr_token;
	Token prev_token;
} Parser;

typedef struct Program {
	struct Stmt **statements;
	int statement_count;
	int statement_capacity;
} Program;

Parser parser_init(struct Lexer *lexer);

Program parser_parse_program(Parser *parser);
