#pragma once

#include "core/common.h"

#include "token.h"

typedef struct Lexer
{
	const char *input;

	const char *start;
	const char *current;
} Lexer;

Lexer lexer_init(const char *source);
void lexer_free(Lexer *lexer);

Token lexer_get_next_token(Lexer *lexer);
