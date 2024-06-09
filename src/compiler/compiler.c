#include "compiler.h"

#include "chunk.h"

#include "core/value.h"
#include "core/common.h"
#include "core/dyn_array.h"

#include "ast/ast.h"
#include "ast/token.h"

#include "debug/debug.h"

static CompileResult compile_stmt(Compiler *compiler, Stmt *stmt);
static CompileResult compile_expr(Compiler *compiler, Expr *expr);

void compiler_init(Compiler *compiler, Chunk *chunk)
{
	compiler->chunk = chunk;
}

void compiler_free(Compiler *compiler)
{
	compiler->chunk = NULL;
}

CompileResult compile_program(Compiler *compiler, Program program)
{
	int count = darray_len(program.statements);

	CompileResult result = COMPILE_OK;

	for (int i = 0; i < count; i++)
	{
		result |= compile_stmt(compiler, program.statements[i]);
	}

	chunk_write(compiler->chunk, OP_RETURN);

	return result;
}

static CompileResult compile_stmt(Compiler *compiler, Stmt *stmt)
{
	CompileResult result = COMPILE_OK;

	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			result = compile_expr(compiler, stmt->expression.expr);
		}
		break;

		default:
		{
			printf("Statement type %s not implemented\n",
				   debug_stmt_type_str(stmt->type));
			UNREACHABLE();
		}
	}

	return result;
}

static CompileResult compile_binary_expr(Chunk *chunk, BinaryExpr expr);
static CompileResult compile_unary_expr(Chunk *chunk, UnaryExpr expr);

static CompileResult compile_expr(Compiler *compiler, Expr *expr)
{
	switch (expr->type)
	{
		case EXPR_NUMBER_LITERAL:
		{
			chunk_write_constant(compiler->chunk, value_number(expr->number));
		}
		break;

		case EXPR_BINARY:
		{
			compile_expr(compiler, expr->binary.left);
			compile_expr(compiler, expr->binary.right);
			compile_binary_expr(compiler->chunk, expr->binary);
		}
		break;

		case EXPR_UNARY:
		{
			compile_expr(compiler, expr->unary.right);
			compile_unary_expr(compiler->chunk, expr->unary);
		}
		break;

		default:
			printf("Expression type %s not implemented\n",
				   debug_expr_type_str(expr->type));
			UNREACHABLE();
	}

	// TODO: Error handling
	return COMPILE_OK;
}

static CompileResult compile_binary_expr(Chunk *chunk, BinaryExpr expr)
{
	switch (expr.op)
	{
		case TOKEN_PLUS:
			chunk_write(chunk, OP_ADD);
			break;

		case TOKEN_MINUS:
			chunk_write(chunk, OP_SUBTRACT);
			break;

		case TOKEN_STAR:
			chunk_write(chunk, OP_MULTIPLY);
			break;

		case TOKEN_SLASH:
			chunk_write(chunk, OP_DIVIDE);
			break;

		default:
			UNREACHABLE();
	}

	return COMPILE_OK;
}

static CompileResult compile_unary_expr(Chunk *chunk, UnaryExpr expr)
{
	switch (expr.op)
	{
		case TOKEN_MINUS:
			chunk_write(chunk, OP_NEGATE);
			break;

		default:
			UNREACHABLE();
	}

	return COMPILE_OK;
}
