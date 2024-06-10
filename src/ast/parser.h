#pragma once

#include "core/hash_table.h"

#include "token.h"

typedef struct Parser
{
	struct Lexer *lexer;

	Token curr_token;
	Token prev_token;

	HashTable strings;
} Parser;

Parser parser_init(struct Lexer *lexer);

struct Program parser_parse_program(Parser *parser);
