#include "ast.h"

#include <string.h>

#include "common.h"
#include "hash_table.h"

Identifier ast_identifier(Hash_Table *table, Token tk)
{
	UNUSED(table);

	Identifier ident = {
		.str = malloc(tk.lexeme_len),
		.len = tk.lexeme_len,
	};

	memcpy(ident.str, tk.lexeme_start, tk.lexeme_len);

	return ident;
}

static Expr *make_expr(Expr_Type type)
{
	Expr *ptr = malloc(sizeof(Expr));

	if (ptr == NULL)
	{
		UNREACHABLE();
	}

	ptr->expr_type = type;
	return ptr;
}

Expr *ast_expr_binary(Token_Type op, Expr *left, Expr *right)
{
	Expr *node = make_expr(Expr_Binary);

	node->binary.op = op;
	node->binary.left = left;
	node->binary.right = right;

	return node;
}

Expr *ast_expr_grouping(Expr *expr)
{
	Expr *node = make_expr(Expr_Grouping);

	node->grouping.expr = expr;

	return node;
}

Expr *ast_expr_unary(Token_Type op, Expr *right)
{
	Expr *node = make_expr(Expr_Unary);

	node->unary.op = op;
	node->unary.right = right;

	return node;
}

Expr *ast_expr_number_literal(double value)
{
	Expr *node = make_expr(Expr_NumberLiteral);

	node->number = value;

	return node;
}

Expr *ast_expr_boolean_literal(bool value)
{
	Expr *node = make_expr(Expr_BooleanLiteral);

	node->boolean = value;

	return node;
}

Expr *ast_expr_identifier(Identifier identifier)
{
	Expr *node = make_expr(Expr_Identifier);

	node->identifier = identifier;

	return node;
}

Expr *ast_expr_assignment(Identifier name, Expr *expr)
{
	Expr *node = make_expr(Expr_Assignment);

	node->assignment.name = name;
	node->assignment.value = expr;

	return node;
}

static Stmt *make_stmt(Stmt_Type type)
{
	Stmt *ptr = malloc(sizeof(Stmt));

	if (ptr == NULL)
	{
		UNREACHABLE();
	}

	ptr->stmt_type = type;
	return ptr;
}

Stmt *ast_stmt_expression(Expr *expr)
{
	Stmt *node = make_stmt(Stmt_Expr);

	node->expression.expr = expr;

	return node;
}

Stmt *ast_stmt_print(Expr *expr)
{
	Stmt *node = make_stmt(Stmt_Print);

	node->print.expr = expr;

	return node;
}

Stmt *ast_stmt_var_decl(Identifier identifier, Expr *expr)
{
	Stmt *node = make_stmt(Stmt_VarDecl);

	node->var_decl.identifier = identifier;
	node->var_decl.expr = expr;

	return node;
}

Stmt *ast_stmt_block(Stmt **statements)
{
	Stmt *node = make_stmt(Stmt_Block);

	node->block.statements = statements;

	return node;
}

Stmt *ast_stmt_if(Expr *cond, Stmt *then_branch, Stmt *else_branch)
{
	Stmt *node = make_stmt(Stmt_If);

	node->if_stmt.cond = cond;
	node->if_stmt.then_branch = then_branch;
	node->if_stmt.else_branch = else_branch;

	return node;
}

Stmt *ast_stmt_while(Expr *cond, Stmt *body)
{
	Stmt *node = make_stmt(Stmt_While);

	node->while_stmt.cond = cond;
	node->while_stmt.body = body;

	return node;
}
