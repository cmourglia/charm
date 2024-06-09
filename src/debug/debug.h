#pragma once

#include "core/common.h"

struct Value;
struct Object;
struct Token;
struct Program;
struct Chunk;

enum TokenType;
enum ExprType;
enum StmtType;

void print_value(struct Value *value);
void print_object(struct Object *obj);

const char *debug_get_token_type_str(enum TokenType type);
int debug_token_to_string(char *buffer, int capacity, struct Token token);

const char *debug_expr_type_str(enum ExprType type);
const char *debug_stmt_type_str(enum StmtType type);

void debug_print_program(struct Program program);

void debug_disassemble_chunk(struct Chunk *chunk, const char *name);
i32 debug_disassemble_instruction(struct Chunk *chunk, i32 offset);
