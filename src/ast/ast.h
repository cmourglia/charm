#pragma once

#include "token.h"

#include "core/common.h"

struct Object;

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
	EXPR_OBJECT_LITERAL,
	EXPR_IDENTIFIER,
	EXPR_ASSIGNMENT,
	EXPR_CALL,
} ExprType;

struct Expr;

typedef struct BinaryExpr
{
	struct Expr *left;
	struct Expr *right;
	TokenType op;
} BinaryExpr;

typedef struct GroupingExpr
{
	struct Expr *expr;
} GroupingExpr;

typedef struct UnaryExpr
{
	struct Expr *right;
	TokenType op;
} UnaryExpr;

typedef struct AssignmentExpr
{
	Identifier name;
	struct Expr *value;
} AssignmentExpr;

typedef struct CallExpr
{
	struct Expr *callee;
	struct Expr **arguments;
} CallExpr;

typedef struct Expr
{
	union
	{
		BinaryExpr binary;
		GroupingExpr grouping;
		UnaryExpr unary;
		AssignmentExpr assignment;
		CallExpr call;
		double number;
		bool boolean;
		struct Object *object;
		Identifier identifier; // TODO: ObjString ?
	} as;

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

typedef struct ExprStmt
{
	Expr *expr;
} ExprStmt;

typedef struct VarDecl
{
	Identifier name;
	Expr *expr;
} VarDecl;

typedef struct FunctionDecl
{
	Identifier name;
	Identifier *args;
	struct Stmt *body;
} FunctionDecl;

typedef struct IfStmt
{
	Expr *cond;
	struct Stmt *then_branch;
	struct Stmt *else_branch;
} IfStmt;

typedef struct BlockStmt
{
	struct Stmt **statements;
} BlockStmt;

typedef struct WhileStmt
{
	Expr *cond;
	struct Stmt *body;
} WhileStmt;

typedef struct ReturnStmt
{
	Expr *expr;
} ReturnStmt;

typedef struct Stmt
{
	union
	{
		ExprStmt expression;
		VarDecl var_decl;
		FunctionDecl function_decl;
		IfStmt if_stmt;
		BlockStmt block;
		WhileStmt while_stmt;
		ReturnStmt return_stmt;
	} as;

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
Expr *ast_expr_obj_literal(struct Object *value);
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
