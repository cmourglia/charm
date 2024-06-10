#include "compiler.h"

#include "chunk.h"

#include "core/value.h"
#include "core/common.h"
#include "core/dyn_array.h"

#include "ast/ast.h"
#include "ast/token.h"

#include "debug/debug.h"

#define DEBUG_PRINT_CODE

static Chunk *compiling_chunk;

static Chunk *current_chunk()
{
	return compiling_chunk;
}

static void emit_byte(u8 byte)
{
	chunk_write(current_chunk(), byte);
}

static void emit_bytes(u8 b1, u8 b2)
{
	chunk_write(current_chunk(), b1);
	chunk_write(current_chunk(), b2);
}

static void emit_constant(Value constant)
{
	chunk_write_constant(current_chunk(), constant);
}

static CompileResult compile_stmt(Stmt *stmt);
static CompileResult compile_expr(Expr *expr);

CompileResult compile_program(struct Chunk *chunk, Program program)
{
	compiling_chunk = chunk;

	int count = darray_len(program.statements);

	CompileResult result = COMPILE_OK;

	for (int i = 0; i < count; i++)
	{
		result |= compile_stmt(program.statements[i]);
	}

	emit_byte(OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	printf("\n-*-*-*- Compiled Bytecode -*-*-*-\n");
	debug_disassemble_chunk(current_chunk(), "code");
#endif

	return result;
}

static CompileResult compile_stmt(Stmt *stmt)
{
	CompileResult result = COMPILE_OK;

	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			result = compile_expr(stmt->as.expression.expr);
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

static CompileResult compile_binary_expr(BinaryExpr expr);
static CompileResult compile_unary_expr(UnaryExpr expr);

static CompileResult compile_expr(Expr *expr)
{
	switch (expr->type)
	{
		case EXPR_BOOLEAN_LITERAL:
		{
			bool b = expr->as.boolean;
			emit_byte(b ? OP_TRUE : OP_FALSE);
		}
		break;

		case EXPR_NUMBER_LITERAL:
		{
			emit_constant(value_number(expr->as.number));
		}
		break;

		case EXPR_CELL_LITERAL:
		{
			emit_constant(value_cell(expr->as.cell));
		}
		break;

		case EXPR_BINARY:
		{
			compile_expr(expr->as.binary.left);
			compile_expr(expr->as.binary.right);
			compile_binary_expr(expr->as.binary);
		}
		break;

		case EXPR_UNARY:
		{
			compile_expr(expr->as.unary.right);
			compile_unary_expr(expr->as.unary);
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

static CompileResult compile_binary_expr(BinaryExpr expr)
{
	switch (expr.op)
	{
		case TOKEN_PLUS:
			emit_byte(OP_ADD);
			break;
		case TOKEN_MINUS:
			emit_byte(OP_SUBTRACT);
			break;
		case TOKEN_STAR:
			emit_byte(OP_MULTIPLY);
			break;
		case TOKEN_SLASH:
			emit_byte(OP_DIVIDE);
			break;
		case TOKEN_EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case TOKEN_BANG_EQUAL:
			emit_bytes(OP_EQUAL, OP_NOT);
			break;
		case TOKEN_GREATER:
			emit_byte(OP_GREATER);
			break;
		case TOKEN_GREATER_EQUAL:
			emit_bytes(OP_LESS, OP_NOT);
			break;
		case TOKEN_LESS:
			emit_byte(OP_LESS);
			break;
		case TOKEN_LESS_EQUAL:
			emit_bytes(OP_GREATER, OP_NOT);
			break;

		default:
			UNREACHABLE();
	}

	return COMPILE_OK;
}

static CompileResult compile_unary_expr(UnaryExpr expr)
{
	switch (expr.op)
	{
		case TOKEN_MINUS:
			emit_byte(OP_NEGATE);
			break;

		case TOKEN_NOT:
			emit_byte(OP_NOT);
			break;

		default:
			UNREACHABLE();
	}

	return COMPILE_OK;
}
