#pragma once

#include "token.h"

typedef enum {
	Expr_Binary,
	Expr_Grouping,
	Expr_Unary,
	Expr_BooleanLiteral,
	Expr_NumberLiteral,
} ExprType;

struct Expr;

typedef struct Expr {
	union {
		struct {
			TokenType op;
			struct Expr *left;
			struct Expr *right;
		} binary;

		struct {
			struct Expr *expr;
		} grouping;

		struct {
			TokenType op;
			struct Expr *right;
		} unary;

		double number;
		bool boolean;
	};

	ExprType expr_type;
} Expr;

//typedef enum StmtType {
//	Stmt_If,
//} StmtType;

//typedef struct Stmt {
//	StmtType stmt_type;
//} Stmt;

Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right);
Expr *ast_expr_grouping(Expr *expr);
Expr *ast_expr_unary(TokenType op, Expr *right);
Expr *ast_expr_number_literal(double value);
Expr *ast_expr_boolean_literal(bool value);

//void ast_print_stmt(Stmt *stmt);
