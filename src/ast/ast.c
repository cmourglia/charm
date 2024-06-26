#include "ast/ast.h"

#include <string.h>

#include "core/common.h"

static Expr *make_expr(ExprType type)
{
	Expr *ptr = malloc(sizeof(Expr));

	if (ptr == NULL)
	{
		UNREACHABLE();
	}

	ptr->type = type;
	return ptr;
}

Expr *ast_expr_binary(TokenType op, Expr *left, Expr *right)
{
	Expr *node = make_expr(EXPR_BINARY);

	node->as.binary = (BinaryExpr){
		.op = op,
		.left = left,
		.right = right,
	};

	return node;
}

Expr *ast_expr_grouping(Expr *expr)
{
	Expr *node = make_expr(EXPR_GROUPING);

	node->as.grouping.expr = expr;

	return node;
}

Expr *ast_expr_unary(TokenType op, Expr *right)
{
	Expr *node = make_expr(EXPR_UNARY);

	node->as.unary = (UnaryExpr){
		.op = op,
		.right = right,
	};

	return node;
}

Expr *ast_expr_number_literal(double value)
{
	Expr *node = make_expr(EXPR_NUMBER_LITERAL);

	node->as.number = value;

	return node;
}

Expr *ast_expr_boolean_literal(bool value)
{
	Expr *node = make_expr(EXPR_BOOLEAN_LITERAL);

	node->as.boolean = value;

	return node;
}

Expr *ast_expr_cell_literal(struct Cell *value)
{
	Expr *node = make_expr(EXPR_CELL_LITERAL);

	node->as.cell = value;

	return node;
}

Expr *ast_expr_identifier(String *identifier)
{
	Expr *node = make_expr(EXPR_IDENTIFIER);

	node->as.identifier = identifier;

	return node;
}

Expr *ast_expr_assignment(String *name, Expr *expr)
{
	Expr *node = make_expr(EXPR_ASSIGNMENT);

	node->as.assignment = (AssignmentExpr){
		.name = name,
		.value = expr,
	};

	return node;
}

Expr *ast_expr_call(Expr *callee, Expr **arguments)
{
	Expr *node = make_expr(EXPR_CALL);

	node->as.call = (CallExpr){
		.callee = callee,
		.arguments = arguments,
	};

	return node;
}

static Stmt *make_stmt(StmtType type)
{
	Stmt *ptr = malloc(sizeof(Stmt));

	if (ptr == NULL)
	{
		UNREACHABLE();
	}

	ptr->type = type;
	return ptr;
}

Stmt *ast_stmt_expression(Expr *expr)
{
	Stmt *node = make_stmt(STMT_EXPR);

	node->as.expression.expr = expr;

	return node;
}

Stmt *ast_stmt_var_decl(String *name, Expr *expr)
{
	Stmt *node = make_stmt(STMT_VAR_DECL);

	node->as.var_decl = (VarDecl){
		.name = name,
		.expr = expr,
	};

	return node;
}

Stmt *ast_stmt_function_decl(String *name, String **args, Stmt *body)
{
	Stmt *node = make_stmt(STMT_FUNCTION_DECL);

	node->as.function_decl = (FunctionDecl){
		.name = name,
		.args = args,
		.body = body,
	};

	return node;
}

Stmt *ast_stmt_block(Stmt **statements)
{
	Stmt *node = make_stmt(STMT_BLOCK);

	node->as.block.statements = statements;

	return node;
}

Stmt *ast_stmt_if(Expr *cond, Stmt *then_branch, Stmt *else_branch)
{
	Stmt *node = make_stmt(STMT_IF);

	node->as.if_stmt = (IfStmt){
		.cond = cond,
		.then_branch = then_branch,
		.else_branch = else_branch,
	};

	return node;
}

Stmt *ast_stmt_while(Expr *cond, Stmt *body)
{
	Stmt *node = make_stmt(STMT_WHILE);

	node->as.while_stmt = (WhileStmt){
		.cond = cond,
		.body = body,
	};

	return node;
}

Stmt *ast_stmt_return(Expr *expr)
{
	Stmt *node = make_stmt(STMT_RETURN);

	node->as.return_stmt.expr = expr;

	return node;
}
