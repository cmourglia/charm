#pragma once

#include "token.h"

#include "core/common.h"
#include "core/string.h"

typedef struct Identifier
{
	char *str;
	int len;
} Identifier;

typedef enum ExprType
{
	EXPR_BINARY,
	EXPR_GROUPING,
	EXPR_UNARY,
	EXPR_BOOLEAN_LITERAL,
	EXPR_NUMBER_LITERAL,
	EXPR_STRING_LITERAL,
	EXPR_IDENTIFIER,
	EXPR_ASSIGNMENT,
	EXPR_CALL,
} ExprType;

struct Expr;

typedef struct Expr
{
	union
	{
		struct
		{
			struct Expr *left;
			struct Expr *right;
			Token_Type op;
		} binary;

		struct
		{
			struct Expr *expr;
		} grouping;

		struct
		{
			struct Expr *right;
			Token_Type op;
		} unary;

		struct
		{
			Identifier name;
			struct Expr *value;
		} assignment;

		struct
		{
			struct Expr *callee;
			struct Expr **arguments;
		} call;

		double number;
		bool boolean;
		String string;
		Identifier identifier;
	};

	ExprType type;
} Expr;

typedef enum StmtType
{
	STMT_EXPR,
	STMT_VAR_DECL,
	STMT_FUNCTION_DECL,
	STMT_BLOCK,
	STMT_IF,
	STMT_WHILE,
	STMT_RETURN,
} StmtType;

struct Stmt;

typedef struct Stmt
{
	union
	{
		struct
		{
			Expr *expr;
		} expression;

		struct
		{
			Identifier name;
			Expr *expr;
		} var_decl;

		struct
		{
			Identifier name;
			Identifier *args;
			struct Stmt *body;
		} function_decl;

		struct
		{
			Expr *cond;
			struct Stmt *then_branch;
			struct Stmt *else_branch;
		} if_stmt;

		struct
		{
			struct Stmt **statements;
		} block;

		struct
		{
			Expr *cond;
			struct Stmt *body;
		} while_stmt;

		struct
		{
			Expr *expr;
		} return_stmt;
	};

	StmtType type;
} Stmt;

typedef struct Program
{
	Stmt **statements;
} Program;

struct HashTable;
Identifier ast_identifier(struct HashTable *table, Token tk);
Identifier ast_identifier_from_cstr(struct HashTable *table, const char *str);

Expr *ast_expr_number_literal(double value);
Expr *ast_expr_boolean_literal(bool value);
Expr *ast_expr_string_literal(String value);
Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right);
Expr *ast_expr_grouping(Expr *expr);
Expr *ast_expr_unary(TokenType op, Expr *right);
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
