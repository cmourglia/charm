#pragma once

#include "ast/token.h"

const char *debug_get_token_type_str(Token_Type type);

int debug_token_to_string(char *buffer, int capacity, Token token);
