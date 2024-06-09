#pragma once

#include "ast/token.h"

struct Value;
void print_value(struct Value *value);

const char *debug_get_token_type_str(TokenType type);
int debug_token_to_string(char *buffer, int capacity, Token token);

struct Program;
void debug_print_program(struct Program program);

struct Chunk;
void debug_disassemble_chunk(struct Chunk *chunk, const char *name);
