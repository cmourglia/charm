#pragma once

#include "token.h"

typedef struct Identifier {
	char *str;
	int len;
} Identifier;

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
			struct Expr *left;
			struct Expr *right;
			TokenType op;
		} binary;

		struct {
			struct Expr *expr;
		} grouping;

		struct {
			struct Expr *right;
			TokenType op;
		} unary;

		double number;
		bool boolean;
	};

	ExprType expr_type;
} Expr;

typedef enum StmtType {
	Stmt_Expr,
	Stmt_Print,
	Stmt_VarDecl,
} StmtType;

struct Stmt;

typedef struct Stmt {
	union {
		struct {
			Expr *expr;
		} expression;

		struct {
			Expr *expr;
		} print;

		struct {
			Identifier identifier;
			Expr *expr;
		} var_decl;
	};

	StmtType stmt_type;
} Stmt;

// Allocs memory
Identifier ast_identifier(Token tk);

Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right);
Expr *ast_expr_grouping(Expr *expr);
Expr *ast_expr_unary(TokenType op, Expr *right);
Expr *ast_expr_number_literal(double value);
Expr *ast_expr_boolean_literal(bool value);

Stmt *ast_stmt_expression(Expr *expr);
Stmt *ast_stmt_print(Expr *expr);
Stmt *ast_stmt_var_decl(Identifier identifier, Expr *expr);
