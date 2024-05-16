#pragma once

#include "token.h"

struct Program;

const char *debug_get_token_type_str(TokenType type);

int debug_token_to_string(char *buffer, int capacity, Token token);

void debug_print_program(struct Program program);
