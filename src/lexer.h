#pragma once

#include "common.h"
#include "token.h"

typedef struct {
	const char *input;

	const char *start;
	const char *current;
} Lexer;

Lexer lexer_init(const char *source);
void lexer_free(Lexer *lexer);

Token lexer_get_next_token(Lexer *lexer);
