#pragma once

#include "token.h"
#include "hash_table.h"

typedef struct Parser {
	struct Lexer *lexer;

	Token curr_token;
	Token prev_token;

	Hash_Table identifiers;
} Parser;

typedef struct Program {
	Stmt **statements;
} Program;

Parser parser_init(struct Lexer *lexer);

Program parser_parse_program(Parser *parser);
