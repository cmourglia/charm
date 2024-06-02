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
	Expr_Identifier,
	Expr_Assignment, // TODO: Make this an expression at some point
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

		struct {
			Identifier name;
			struct Expr *value;
		} assignment;

		double number;
		bool boolean;
		Identifier identifier;
	};

	ExprType expr_type;
} Expr;

typedef enum StmtType {
	Stmt_Expr,
	Stmt_Print,
	Stmt_VarDecl,
	Stmt_Block,
	Stmt_If,
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

		struct {
			Expr *cond;
			struct Stmt *then_branch;
			struct Stmt *else_branch;
		} if_stmt;

		struct {
			struct Stmt **statements;
		} block;
	};

	StmtType stmt_type;
} Stmt;

struct HashTable;
Identifier ast_identifier(struct HashTable *table, Token tk);

Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right);
Expr *ast_expr_grouping(Expr *expr);
Expr *ast_expr_unary(TokenType op, Expr *right);
Expr *ast_expr_number_literal(double value);
Expr *ast_expr_boolean_literal(bool value);
Expr *ast_expr_identifier(Identifier identifier);
Expr *ast_expr_assignment(Identifier identifier, Expr *value);

Stmt *ast_stmt_expression(Expr *expr);
Stmt *ast_stmt_print(Expr *expr);
Stmt *ast_stmt_var_decl(Identifier identifier, Expr *expr);
Stmt *ast_stmt_block(Stmt **statements);
Stmt *ast_stmt_if(Expr *cond, Stmt *then_branch, Stmt *else_branch);
