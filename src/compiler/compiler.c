#include "compiler.h"

#include <stdint.h>
#include <stdarg.h>

#include "core/value.h"
#include "core/common.h"
#include "core/dyn_array.h"

#include "ast/ast.h"
#include "ast/token.h"

#include "compiler/chunk.h"

#include "debug/debug.h"

#define DEBUG_PRINT_CODE

#define UINT8_COUNT UINT8_MAX + 1

typedef struct
{
	String *name;
	i32 depth;
} Local;

typedef struct
{
	Local locals[UINT8_COUNT];
	i32 local_count;
	i32 scope_depth;
} Compiler;

static Chunk *compiling_chunk;
static Compiler compiler;

static Chunk *current_chunk()
{
	return compiling_chunk;
}

static void emit_byte(u8 byte)
{
	chunk_write(current_chunk(), byte);
}

static void emit_bytes(i32 count, ...)
{
	va_list args;
	va_start(args, count);
	for (i32 i = 0; i < count; i++)
	{
		chunk_write(current_chunk(), (u8)va_arg(args, int));
	}
	va_end(args);
}

static usize emit_jump(u8 instruction)
{
	emit_bytes(3, instruction, 0xFF, 0xFF);
	return arrlen(current_chunk()->code) - 2;
}

static void patch_jump(usize offset)
{
	usize jump = arrlen(current_chunk()->code) - offset - 2;

	assert(jump <= UINT16_MAX);

	current_chunk()->code[offset] = (jump >> 8) & 0xFF;
	current_chunk()->code[offset + 1] = jump & 0xFF;
}

static void emit_loop(usize loop_start)
{
	emit_byte(OP_LOOP);

	usize offset = arrlen(current_chunk()->code) - loop_start + 2;
	assert(offset <= UINT16_MAX);

	emit_bytes(2, (offset >> 8) & 0xFF, offset & 0xFF);
}

static u16 make_constant(Value constant)
{
	u16 id = chunk_add_constant(current_chunk(), constant);
	assert(id < 256 && "TODO: Handle more than 256 constants");
	return id;
}

static void emit_constant(Value constant)
{
	emit_bytes(2, OP_CONSTANT, make_constant(constant));
}

static u16 identifier_constant(String *identifier)
{
	return make_constant(value_cell((Cell *)identifier));
}

//static void define_variable(u8 global_variable);
//static u8 identifier_constant();

static CompileResult compile_stmt(Stmt *stmt);
static CompileResult compile_expr(Expr *expr);

CompileResult compile_program(struct Chunk *chunk, Program program)
{
	compiler = (Compiler){ .local_count = 0, .scope_depth = 0 };
	compiling_chunk = chunk;

	i32 count = (i32)arrlen(program.statements);

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

static void mark_initialized()
{
	if (compiler.scope_depth == 0)
	{
		return;
	}

	compiler.locals[compiler.local_count - 1].depth = compiler.scope_depth;
}

static void define_variable(u16 identifier)
{
	if (compiler.scope_depth > 0)
	{
		mark_initialized();
		return;
	}

	emit_bytes(2, OP_DEFINE_GLOBAL, identifier);
}

static void add_local(String *name)
{
	if (compiler.local_count == UINT8_COUNT)
	{
		printf("Too many locals in function\n");
		return;
	}

	Local *local = &compiler.locals[compiler.local_count++];
	local->name = name;
	local->depth = -1;
}

static void declare_variable(String *name)
{
	if (compiler.scope_depth == 0)
	{
		return;
	}

	add_local(name);
}

static void begin_scope()
{
	compiler.scope_depth += 1;
}

static void end_scope()
{
	compiler.scope_depth -= 1;

	while (compiler.local_count > 0 &&
		   compiler.locals[compiler.local_count - 1].depth >
			   compiler.scope_depth)
	{
		emit_byte(OP_POP);
		compiler.local_count -= 1;
	}
}

static CompileResult compile_stmt(Stmt *stmt)
{
	CompileResult result = COMPILE_OK;

	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			result = compile_expr(stmt->as.expression.expr);
			emit_byte(OP_POP);
		}
		break;

		case STMT_VAR_DECL:
		{
			String *name = stmt->as.var_decl.name;

			declare_variable(name);

			u8 ident = 0;
			if (compiler.scope_depth == 0)
			{
				ident = (u8)identifier_constant(name);
			}

			if (stmt->as.var_decl.expr != NULL)
			{
				compile_expr(stmt->as.var_decl.expr);
			}
			else
			{
				emit_byte(OP_NIL);
			}

			define_variable(ident);
		}
		break;

		case STMT_BLOCK:
		{
			begin_scope();

			for (int i = 0; i < arrlen(stmt->as.block.statements); i++)
			{
				compile_stmt(stmt->as.block.statements[i]);
			}

			end_scope();
		}
		break;

		case STMT_IF:
		{
			compile_expr(stmt->as.if_stmt.cond);

			usize then_jump = emit_jump(OP_JUMP_IF_FALSE);
			emit_byte(OP_POP);

			compile_stmt(stmt->as.if_stmt.then_branch);

			usize else_jump = emit_jump(OP_JUMP);

			patch_jump(then_jump);

			emit_byte(OP_POP);

			if (stmt->as.if_stmt.else_branch != NULL)
			{
				compile_stmt(stmt->as.if_stmt.else_branch);
			}

			patch_jump(else_jump);
		}
		break;

		case STMT_WHILE:
		{
			usize loop_start = arrlen(current_chunk()->code);
			compile_expr(stmt->as.while_stmt.cond);

			usize exit_jump = emit_jump(OP_JUMP_IF_FALSE);
			emit_byte(OP_POP);

			compile_stmt(stmt->as.while_stmt.body);

			emit_loop(loop_start);

			patch_jump(exit_jump);
			emit_byte(OP_POP);
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

static u16 resolve_local(String *name)
{
	for (int i = compiler.local_count - 1; i >= 0; i--)
	{
		Local *local = &compiler.locals[i];
		if (name == local->name)
		{
			return (u16)i;
		}
	}

	return (u16)-1;
}

static void named_variable(String *name, bool assignment)
{
	u8 get_op, set_op;

	u16 arg = resolve_local(name);

	if (arg != (u16)-1)
	{
		get_op = OP_GET_LOCAL;
		set_op = OP_SET_LOCAL;
	}
	else
	{
		arg = identifier_constant(name);
		get_op = OP_GET_GLOBAL;
		set_op = OP_SET_GLOBAL;
	}

	emit_bytes(2, assignment ? set_op : get_op, arg);
}

static CompileResult compile_binary_expr(BinaryExpr expr);
static CompileResult compile_unary_expr(UnaryExpr expr);

static CompileResult compile_expr(Expr *expr)
{
	switch (expr->type)
	{
		case EXPR_IDENTIFIER:
		{
			named_variable(expr->as.identifier, false);
		}
		break;

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
			compile_binary_expr(expr->as.binary);
		}
		break;

		case EXPR_UNARY:
		{
			compile_expr(expr->as.unary.right);
			compile_unary_expr(expr->as.unary);
		}
		break;

		case EXPR_ASSIGNMENT:
		{
			compile_expr(expr->as.assignment.value);
			named_variable(expr->as.assignment.name, true);
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
	compile_expr(expr.left);

	switch (expr.op)
	{
		case TOKEN_PLUS:
			compile_expr(expr.right);
			emit_byte(OP_ADD);
			break;
		case TOKEN_MINUS:
			compile_expr(expr.right);
			emit_byte(OP_SUBTRACT);
			break;
		case TOKEN_STAR:
			compile_expr(expr.right);
			emit_byte(OP_MULTIPLY);
			break;
		case TOKEN_SLASH:
			compile_expr(expr.right);
			emit_byte(OP_DIVIDE);
			break;
		case TOKEN_EQUAL_EQUAL:
			compile_expr(expr.right);
			emit_byte(OP_EQUAL);
			break;
		case TOKEN_BANG_EQUAL:
			compile_expr(expr.right);
			emit_bytes(2, OP_EQUAL, OP_NOT);
			break;
		case TOKEN_GREATER:
			compile_expr(expr.right);
			emit_byte(OP_GREATER);
			break;
		case TOKEN_GREATER_EQUAL:
			compile_expr(expr.right);
			emit_bytes(2, OP_LESS, OP_NOT);
			break;
		case TOKEN_LESS:
			compile_expr(expr.right);
			emit_byte(OP_LESS);
			break;
		case TOKEN_LESS_EQUAL:
			compile_expr(expr.right);
			emit_bytes(2, OP_GREATER, OP_NOT);
			break;
		case TOKEN_AND:
		{
			usize end_jump = emit_jump(OP_JUMP_IF_FALSE);
			emit_byte(OP_POP);
			compile_expr(expr.right);
			patch_jump(end_jump);
		}
		break;

		case TOKEN_OR:
		{
			usize else_jump = emit_jump(OP_JUMP_IF_FALSE);
			usize end_jump = emit_jump(OP_JUMP);

			patch_jump(else_jump);
			emit_byte(OP_POP);
			compile_expr(expr.right);
			patch_jump(end_jump);
		}
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
