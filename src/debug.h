#pragma once

struct Token;
struct Expr;

int debug_token_to_string(char *buffer, int capacity, struct Token token);

void debug_print_expr(struct Expr *expr);
