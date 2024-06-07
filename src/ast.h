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
	Expr_Call,
} Expr_Type;

struct Expr;

typedef struct Expr {
	union {
		struct {
			struct Expr *left;
			struct Expr *right;
			Token_Type op;
		} binary;

		struct {
			struct Expr *expr;
		} grouping;

		struct {
			struct Expr *right;
			Token_Type op;
		} unary;

		struct {
			Identifier name;
			struct Expr *value;
		} assignment;

		struct {
			struct Expr *callee;
			struct Expr **arguments;
		} call;

		double number;
		bool boolean;
		Identifier identifier;
	};

	Expr_Type expr_type;
} Expr;

typedef enum Stmt_Type {
	Stmt_Expr,
	Stmt_VarDecl,
	Stmt_FunctionDecl,
	Stmt_Block,
	Stmt_If,
	Stmt_While,
	Stmt_Return,
} Stmt_Type;

struct Stmt;

typedef struct Stmt {
	union {
		struct {
			Expr *expr;
		} expression;

		struct {
			Identifier name;
			Expr *expr;
		} var_decl;

		struct {
			Identifier name;
			Identifier *args;
			struct Stmt *body;
		} function_decl;

		struct {
			Expr *cond;
			struct Stmt *then_branch;
			struct Stmt *else_branch;
		} if_stmt;

		struct {
			struct Stmt **statements;
		} block;

		struct {
			Expr *cond;
			struct Stmt *body;
		} while_stmt;

		struct {
			Expr *expr;
		} return_stmt;
	};

	Stmt_Type stmt_type;
} Stmt;

struct Hash_Table;
Identifier ast_identifier(struct Hash_Table *table, Token tk);
Identifier ast_identifier_from_cstr(struct Hash_Table *table, const char *str);

Expr *ast_expr_binary(Token_Type op, Expr *left, Expr *right);
Expr *ast_expr_grouping(Expr *expr);
Expr *ast_expr_unary(Token_Type op, Expr *right);
Expr *ast_expr_number_literal(double value);
Expr *ast_expr_boolean_literal(bool value);
Expr *ast_expr_identifier(Identifier identifier);
Expr *ast_expr_assignment(Identifier name, Expr *value);
Expr *ast_expr_call(Expr *callee, Expr **arguments);

Stmt *ast_stmt_expression(Expr *expr);
Stmt *ast_stmt_var_decl(Identifier identifier, Expr *expr);
Stmt *ast_stmt_function_decl(Identifier name, Identifier *args, Stmt *body);
Stmt *ast_stmt_block(Stmt **statements);
Stmt *ast_stmt_if(Expr *cond, Stmt *then_branch, Stmt *else_branch);
Stmt *ast_stmt_while(Expr *cond, Stmt *body);
Stmt *ast_stmt_return(Expr *expr);
