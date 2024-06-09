#include "treewalk.h"

#include <time.h>

#include "core/common.h"
#include "core/value.h"
#include "core/dyn_array.h"

#include "ast/parser.h"
#include "ast/ast.h"
#include "ast/token.h"

#include "debug/debug.h"

#include "frame.h"

static Value interpret_expr(Expr *expr);

static NODISCARD Result interpret_stmt(Stmt *stmt);

static FrameStack frame_stack;
static Identifier native_call_args_name;

static Result native_print(Value *args);
static Result native_time(Value *args);

static void register_native_functions()
{
	native_call_args_name = ast_identifier_from_cstr(NULL, "native_call_args");
	frame_stack_declare_variable(&frame_stack,
								 ast_identifier_from_cstr(NULL, "time"),
								 value_native_function(native_time));

	frame_stack_declare_variable(&frame_stack,
								 ast_identifier_from_cstr(NULL, "print"),
								 value_native_function(native_print));
}

static Result native_time(Value *args)
{
	struct timespec clock;
	clock_gettime(CLOCK_REALTIME, &clock);

	f64 t = (f64)clock.tv_nsec * 1e-9 + clock.tv_sec;

	return result_return(value_number(t));
}

static Result native_print(Value *args)
{
	for (int i = 0; i < darray_len(args); i++)
	{
		Value value = args[i];

		if (i != 0)
		{
			printf(" ");
		}

		print_value(&value);
	}

	printf("\n");

	return result_none();
}

void treewalk_interpreter_run(struct Program program)
{
	frame_stack_init(&frame_stack);
	frame_stack_push_frame(&frame_stack);

	register_native_functions();

	int count = darray_len(program.statements);
	for (int i = 0; i < count; i++)
	{
		Result result = interpret_stmt(program.statements[i]);
		// TODO: Handle errors here ?
		UNUSED(result);
	}
}

static Value mult(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER)
	{
		return value_number(l.number * r.number);
	}

	UNREACHABLE();
}

static Value divide(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER)
	{
		return value_number(l.number / r.number);
	}

	UNREACHABLE();
}

static Value add(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER)
	{
		return value_number(l.number + r.number);
	}

	printf("%u %u\n", l.type, r.type);
	UNREACHABLE();
}

static Value subtract(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.type == VALUE_NUMBER && r.type == VALUE_NUMBER)
	{
		return value_number(l.number - r.number);
	}

	UNREACHABLE();
}

#define BIN_COMP(op)                                           \
	do                                                         \
	{                                                          \
		Value l = interpret_expr(lhs);                         \
		Value r = interpret_expr(rhs);                         \
                                                               \
		if (l.type == r.type)                                  \
		{                                                      \
			switch (l.type)                                    \
			{                                                  \
				case VALUE_NUMBER:                             \
					return value_bool(l.number op r.number);   \
				case VALUE_BOOL:                               \
					return value_bool(l.boolean op r.boolean); \
				default:                                       \
					UNREACHABLE();                             \
			}                                                  \
		}                                                      \
                                                               \
		UNREACHABLE();                                         \
	} while (false)

static Value eq(Expr *lhs, Expr *rhs)
{
	BIN_COMP(==);
}

static Value neq(Expr *lhs, Expr *rhs)
{
	BIN_COMP(!=);
}

static Value lt(Expr *lhs, Expr *rhs)
{
	BIN_COMP(<);
}

static Value gt(Expr *lhs, Expr *rhs)
{
	BIN_COMP(>);
}

static Value leq(Expr *lhs, Expr *rhs)
{
	BIN_COMP(<=);
}

static Value geq(Expr *lhs, Expr *rhs)
{
	BIN_COMP(>=);
}

static Value logic_and(Expr *lhs, Expr *rhs)
{
	Value left = interpret_expr(lhs);
	if (left.type != VALUE_BOOL)
	{
		printf("Operands to `and` must be of type boolean\n");
		// TODO: Proper error handling
		return value_bool(false);
	}

	if (!left.boolean)
	{
		return left;
	}

	Value right = interpret_expr(rhs);
	if (right.type != VALUE_BOOL)
	{
		printf("Operands to `and` must be of type boolean\n");
		return value_bool(false);
	}

	return right;
}

static Value logic_or(Expr *lhs, Expr *rhs)
{
	Value left = interpret_expr(lhs);
	if (left.type != VALUE_BOOL)
	{
		printf("Operands to `or` must be of type boolean\n");
		// TODO: Proper error handling
		return value_bool(false);
	}

	if (left.boolean)
	{
		return left;
	}

	Value right = interpret_expr(rhs);
	if (right.type != VALUE_BOOL)
	{
		printf("Operands to `or` must be of type boolean\n");
		return value_bool(false);
	}

	return right;
}

static Result call_function(FrameStack *stack, Value callee, Value *args);
static Result call_native_function(FrameStack *stack, Value callee,
								   Value *args);

static Value interpret_expr(Expr *expr)
{
	switch (expr->type)
	{
		case EXPR_BOOLEAN_LITERAL:
			return value_bool(expr->boolean);

		case EXPR_NUMBER_LITERAL:
			return value_number(expr->number);

		case EXPR_STRING_LITERAL:
			return value_string(expr->string);

		case EXPR_GROUPING:
			return interpret_expr(expr->grouping.expr);

		case EXPR_BINARY:
		{
			switch (expr->binary.op)
			{
				case TOKEN_MINUS:
					return subtract(expr->binary.left, expr->binary.right);
				case TOKEN_PLUS:
					return add(expr->binary.left, expr->binary.right);
				case TOKEN_SLASH:
					return divide(expr->binary.left, expr->binary.right);
				case TOKEN_STAR:
					return mult(expr->binary.left, expr->binary.right);
				case TOKEN_BANG_EQUAL:
					return neq(expr->binary.left, expr->binary.right);
				case TOKEN_EQUAL_EQUAL:
					return eq(expr->binary.left, expr->binary.right);
				case TOKEN_GREATER:
					return gt(expr->binary.left, expr->binary.right);
				case TOKEN_GREATER_EQUAL:
					return geq(expr->binary.left, expr->binary.right);
				case TOKEN_LESS:
					return lt(expr->binary.left, expr->binary.right);
				case TOKEN_LESS_EQUAL:
					return leq(expr->binary.left, expr->binary.right);
				case TOKEN_AND:
					return logic_and(expr->binary.left, expr->binary.right);
				case TOKEN_OR:
					return logic_or(expr->binary.left, expr->binary.right);

				default:
					UNREACHABLE();
			}
		}
		break;

		case EXPR_UNARY:
		{
			switch (expr->unary.op)
			{
				case TOKEN_MINUS:
				{
					Value v = interpret_expr(expr->unary.right);
					if (v.type == VALUE_NUMBER)
					{
						return value_number(-v.number);
					}

					UNREACHABLE();
				}
				break;

				case TOKEN_NOT:
				{
					Value v = interpret_expr(expr->unary.right);
					if (v.type == VALUE_BOOL)
					{
						return value_bool(!v.boolean);
					}

					UNREACHABLE();
				}
				break;

				default:
					UNREACHABLE();
			}
		}
		break;

		case EXPR_ASSIGNMENT:
		{
			Value value = interpret_expr(expr->assignment.value);
			if (!frame_stack_set_variable(&frame_stack, expr->assignment.name,
										  value))
			{
				// TODO: Error
				printf("Variable '%.*s' does not exist or types do not match\n",
					   expr->assignment.name.len, expr->assignment.name.str);
			}
			return value;
		}
		break;

		case EXPR_IDENTIFIER:
		{
			Value value = value_nil();
			if (!frame_stack_get_value(&frame_stack, expr->identifier, &value))
			{
				// TODO: Error
				printf("Variable '%.*s' does not exist\n",
					   expr->assignment.name.len, expr->assignment.name.str);
			}
			return value;
		}
		break;

		case EXPR_CALL:
		{
			Value callee = interpret_expr(expr->call.callee);

			Value *args = NULL;

			for (int i = 0; i < darray_len(expr->call.arguments); i++)
			{
				darray_push(args, interpret_expr(expr->call.arguments[i]));
			}

			frame_stack_push_frame(&frame_stack);

			Result result = result_none();

			switch (callee.type)
			{
				case VALUE_FUNCTION:
				{
					result = call_function(&frame_stack, callee, args);
				}
				break;

				case VALUE_NATIVE_FUNCTION:
				{
					result = call_native_function(&frame_stack, callee, args);
				}
				break;

				default:
					UNREACHABLE();
			}

			frame_stack_pop_frame(&frame_stack);

			darray_free(args);

			switch (result.type)
			{
				case RESULT_NONE:
					return value_nil();

				case RESULT_RETURN:
					return result.return_result.value;
			}
		}
		break;
	}

	UNREACHABLE();
}

static Result call_function(FrameStack *stack, Value callee, Value *args)
{
	int arity = darray_len(args);

	assert(arity == darray_len(callee.function.args));

	for (int i = 0; i < arity; i++)
	{
		Identifier arg_name = callee.function.args[i];
		Value arg_value = args[i];

		frame_stack_declare_variable(stack, arg_name, arg_value);
	}

	return interpret_stmt(callee.function.body);
}

static Result call_native_function(FrameStack *stack, Value callee, Value *args)
{
	UNUSED(stack);
	return callee.native_function.function(args);
}

static NODISCARD Result interpret_stmt(Stmt *stmt)
{
	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			Value value = interpret_expr(stmt->expression.expr);
			UNUSED(value);
			return result_none();
		}
		break;

		case STMT_VAR_DECL:
		{
			Value value = value_nil();
			if (stmt->var_decl.expr != NULL)
			{
				value = interpret_expr(stmt->var_decl.expr);
			}

			frame_stack_declare_variable(&frame_stack, stmt->var_decl.name,
										 value);

			return result_none();
		}
		break;

		case STMT_FUNCTION_DECL:
		{
			Value value = value_function(stmt->function_decl.args,
										 stmt->function_decl.body);

			frame_stack_declare_variable(&frame_stack, stmt->function_decl.name,
										 value);

			return result_none();
		}
		break;

		case STMT_BLOCK:
		{
			frame_stack_push_frame(&frame_stack);

			Result block_result = result_none();

			int count = darray_len(stmt->block.statements);
			for (int i = 0; i < count; i++)
			{
				Result result = interpret_stmt(stmt->block.statements[i]);
				if (result.type == RESULT_RETURN)
				{
					block_result = result;
					break;
				}
			}

			frame_stack_pop_frame(&frame_stack);

			return block_result;
		}
		break;

		case STMT_IF:
		{
			Value value = interpret_expr(stmt->if_stmt.cond);
			if (value.type != VALUE_BOOL)
			{
				printf("Error: if condition is not a boolean expression\n");
				// FIXME: Return error ?
				return result_none();
			}

			if (value.boolean)
			{
				return interpret_stmt(stmt->if_stmt.then_branch);
			}
			else
			{
				if (stmt->if_stmt.else_branch != NULL)
				{
					return interpret_stmt(stmt->if_stmt.else_branch);
				}

				return result_none();
			}
		}
		break;

		case STMT_WHILE:
		{
			Result block_result = result_none();

			while (true)
			{
				Value value = interpret_expr(stmt->while_stmt.cond);

				if (value.type != VALUE_BOOL)
				{
					printf(
						"Error: while condition is not a boolean expression\n");
					break;
				}

				if (!value.boolean)
				{
					break;
				}

				Result result = interpret_stmt(stmt->while_stmt.body);
				if (result.type == RESULT_RETURN)
				{
					block_result = result;
					break;
				}
			}

			return block_result;
		}
		break;

		case STMT_RETURN:
		{
			Value result = value_nil();
			if (stmt->return_stmt.expr != NULL)
			{
				result = interpret_expr(stmt->return_stmt.expr);
			}

			return result_return(result);
		}
		break;
	}

	UNREACHABLE();
}
