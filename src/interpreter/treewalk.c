#include "treewalk.h"

#include <time.h>

#include "core/cell.h"
#include "core/hash_table.h"
#include "core/common.h"
#include "core/value.h"
#include "core/dyn_array.h"

#include "ast/ast.h"
#include "ast/token.h"

#include "debug/debug.h"

#include "frame.h"

static Value interpret_expr(Expr *expr);

static NODISCARD Result interpret_stmt(Stmt *stmt);

static FrameStack frame_stack;
static String *native_call_args_name;

static Result native_print(Value *args);
static Result native_time(Value *args);

HashTable *strings = NULL;

static void register_native_functions()
{
#define STR(name) string_from_cstr(strings, name)
	native_call_args_name = STR("native_call_args");
	frame_stack_declare_variable(&frame_stack, STR("time"),
								 value_native_function(native_time));

	frame_stack_declare_variable(&frame_stack, STR("print"),
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

	strings = &program.strings;

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

	if (is_number(l) && is_number(r))
	{
		return value_number(l.as.number * r.as.number);
	}

	UNREACHABLE();
}

static Value divide(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (is_number(l) && is_number(r))
	{
		return value_number(l.as.number / r.as.number);
	}

	UNREACHABLE();
}

static Value add(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (is_number(l) && is_number(r))
	{
		return value_number(l.as.number + r.as.number);
	}

	printf("%u %u\n", l.type, r.type);
	UNREACHABLE();
}

static Value subtract(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (is_number(l) && is_number(r))
	{
		return value_number(l.as.number - r.as.number);
	}

	UNREACHABLE();
}

#define BIN_COMP(op)                                                 \
	do                                                               \
	{                                                                \
		Value l = interpret_expr(lhs);                               \
		Value r = interpret_expr(rhs);                               \
                                                                     \
		if (values_share_type(l, r))                                 \
		{                                                            \
			switch (l.type)                                          \
			{                                                        \
				case VALUE_NUMBER:                                   \
					return value_bool(l.as.number op r.as.number);   \
				case VALUE_BOOL:                                     \
					return value_bool(l.as.boolean op r.as.boolean); \
				default:                                             \
					UNREACHABLE();                                   \
			}                                                        \
		}                                                            \
                                                                     \
		UNREACHABLE();                                               \
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

	if (!left.as.boolean)
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

	if (left.as.boolean)
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

static Value interpret_binary_expr(BinaryExpr *expr);

static Value interpret_expr(Expr *expr)
{
	switch (expr->type)
	{
		case EXPR_BOOLEAN_LITERAL:
			return value_bool(expr->as.boolean);

		case EXPR_NUMBER_LITERAL:
			return value_number(expr->as.number);

		case EXPR_CELL_LITERAL:
			return value_cell(expr->as.cell);

		case EXPR_GROUPING:
			return interpret_expr(expr->as.grouping.expr);

		case EXPR_BINARY:
			return interpret_binary_expr(&expr->as.binary);

		case EXPR_UNARY:
		{
			switch (expr->as.unary.op)
			{
				case TOKEN_MINUS:
				{
					Value v = interpret_expr(expr->as.unary.right);
					if (is_number(v))
					{
						return value_number(-v.as.number);
					}

					UNREACHABLE();
				}
				break;

				case TOKEN_NOT:
				{
					Value v = interpret_expr(expr->as.unary.right);
					if (is_bool(v))
					{
						return value_bool(!v.as.boolean);
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
			Value value = interpret_expr(expr->as.assignment.value);
			if (!frame_stack_set_variable(&frame_stack,
										  expr->as.assignment.name, value))
			{
				// TODO: Error
				printf("Variable '%.*s' does not exist or types do not match\n",
					   expr->as.assignment.name->len,
					   expr->as.assignment.name->str);
			}
			return value;
		}
		break;

		case EXPR_IDENTIFIER:
		{
			Value value = value_nil();
			if (!frame_stack_get_value(&frame_stack, expr->as.identifier,
									   &value))
			{
				// TODO: Error
				printf("Variable '%.*s' does not exist\n",
					   expr->as.assignment.name->len,
					   expr->as.assignment.name->str);
			}
			return value;
		}
		break;

		case EXPR_CALL:
		{
			Value callee = interpret_expr(expr->as.call.callee);

			Value *args = NULL;

			for (int i = 0; i < darray_len(expr->as.call.arguments); i++)
			{
				darray_push(args, interpret_expr(expr->as.call.arguments[i]));
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
					return result.as.return_result;
			}
		}
		break;
	}

	UNREACHABLE();
}

static Value interpret_binary_expr(BinaryExpr *binary)
{
	switch (binary->op)
	{
		case TOKEN_MINUS:
			return subtract(binary->left, binary->right);
		case TOKEN_PLUS:
			return add(binary->left, binary->right);
		case TOKEN_SLASH:
			return divide(binary->left, binary->right);
		case TOKEN_STAR:
			return mult(binary->left, binary->right);
		case TOKEN_BANG_EQUAL:
			return neq(binary->left, binary->right);
		case TOKEN_EQUAL_EQUAL:
			return eq(binary->left, binary->right);
		case TOKEN_GREATER:
			return gt(binary->left, binary->right);
		case TOKEN_GREATER_EQUAL:
			return geq(binary->left, binary->right);
		case TOKEN_LESS:
			return lt(binary->left, binary->right);
		case TOKEN_LESS_EQUAL:
			return leq(binary->left, binary->right);
		case TOKEN_AND:
			return logic_and(binary->left, binary->right);
		case TOKEN_OR:
			return logic_or(binary->left, binary->right);

		default:
			UNREACHABLE();
	}
}

static Result call_function(FrameStack *stack, Value callee, Value *args)
{
	int arity = darray_len(args);

	assert(arity == darray_len(callee.as.function.args));

	for (int i = 0; i < arity; i++)
	{
		String *arg_name = callee.as.function.args[i];
		Value arg_value = args[i];

		frame_stack_declare_variable(stack, arg_name, arg_value);
	}

	return interpret_stmt(callee.as.function.body);
}

static Result call_native_function(FrameStack *stack, Value callee, Value *args)
{
	UNUSED(stack);
	return callee.as.native_function(args);
}

static NODISCARD Result interpret_stmt(Stmt *stmt)
{
	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			Value value = interpret_expr(stmt->as.expression.expr);
			UNUSED(value);
			return result_none();
		}
		break;

		case STMT_VAR_DECL:
		{
			Value value = value_nil();
			if (stmt->as.var_decl.expr != NULL)
			{
				value = interpret_expr(stmt->as.var_decl.expr);
			}

			frame_stack_declare_variable(&frame_stack, stmt->as.var_decl.name,
										 value);

			return result_none();
		}
		break;

		case STMT_FUNCTION_DECL:
		{
			Value value = value_function(stmt->as.function_decl.args,
										 stmt->as.function_decl.body);

			frame_stack_declare_variable(&frame_stack,
										 stmt->as.function_decl.name, value);

			return result_none();
		}
		break;

		case STMT_BLOCK:
		{
			frame_stack_push_frame(&frame_stack);

			Result block_result = result_none();

			int count = darray_len(stmt->as.block.statements);
			for (int i = 0; i < count; i++)
			{
				Result result = interpret_stmt(stmt->as.block.statements[i]);
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
			Value value = interpret_expr(stmt->as.if_stmt.cond);
			if (value.type != VALUE_BOOL)
			{
				printf("Error: if condition is not a boolean expression\n");
				// FIXME: Return error ?
				return result_none();
			}

			if (value.as.boolean)
			{
				return interpret_stmt(stmt->as.if_stmt.then_branch);
			}
			else
			{
				if (stmt->as.if_stmt.else_branch != NULL)
				{
					return interpret_stmt(stmt->as.if_stmt.else_branch);
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
				Value value = interpret_expr(stmt->as.while_stmt.cond);

				if (value.type != VALUE_BOOL)
				{
					printf(
						"Error: while condition is not a boolean expression\n");
					break;
				}

				if (!value.as.boolean)
				{
					break;
				}

				Result result = interpret_stmt(stmt->as.while_stmt.body);
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
			if (stmt->as.return_stmt.expr != NULL)
			{
				result = interpret_expr(stmt->as.return_stmt.expr);
			}

			return result_return(result);
		}
		break;
	}

	UNREACHABLE();
}
