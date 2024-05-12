#include "ast.h"
#include "common.h"

static Expr *new_expr(ExprType type)
{
	Expr *ptr = malloc(sizeof(Expr));

	if (ptr == NULL)
	{
		UNREACHABLE();
	}

	ptr->expr_type = type;
	return ptr;
}

Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right)
{
	Expr *node = new_expr(Expr_Binary);

	node->binary.op = op;
	node->binary.left = left;
	node->binary.right = right;

	return node;
}

Expr *ast_expr_grouping(Expr *expr)
{
	Expr *node = new_expr(Expr_Grouping);

	node->grouping.expr = expr;

	return node;
}

Expr *ast_expr_unary(TokenType op, Expr *right)
{
	Expr *node = new_expr(Expr_Unary);

	node->unary.op = op;
	node->unary.right = right;

	return node;
}

Expr *ast_expr_number_literal(double value)
{
	Expr *node = new_expr(Expr_NumberLiteral);

	node->number = value;

	return node;
}

Expr *ast_expr_boolean_literal(bool value)
{
	Expr *node = new_expr(Expr_BooleanLiteral);

	node->boolean = value;

	return node;
}
