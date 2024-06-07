#include "interpreter.h"

#include "beard_lib.h"
#include "hash_table.h"
#include "parser.h"

#include "ast.h"
#include "common.h"
#include "token.h"
#include "value.h"
#include "frame.h"

// TODO: Statement result
typedef enum {
	Result_None,
	Result_Return,
	// TODO: Continue,
	// TODO: Break,
	// TODO: Error ?
} ResultType;

typedef struct {
	union {
		struct {
			Value value;
		} return_result;
	};

	ResultType result_type;
} Result;

static Value interpret_expr(Expr *expr);

static NODISCARD Result interpret_stmt(Stmt *stmt);

static Result result_none()
{
	return (Result){ .result_type = Result_None };
}

static Result result_return(Value value)
{
	return (Result){
		.return_result = { .value = value },
		.result_type = Result_Return,
	};
}

static Frame_Stack frame_stack;

void interpreter_run(struct Program program)
{
	frame_stack_init(&frame_stack);
	frame_stack_push_frame(&frame_stack);

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

	if (l.value_type == Value_Number && r.value_type == Value_Number)
	{
		return value_number(l.number * r.number);
	}

	UNREACHABLE();
}

static Value divide(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.value_type == Value_Number && r.value_type == Value_Number)
	{
		return value_number(l.number / r.number);
	}

	UNREACHABLE();
}

static Value add(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.value_type == Value_Number && r.value_type == Value_Number)
	{
		return value_number(l.number + r.number);
	}

	UNREACHABLE();
}

static Value subtract(Expr *lhs, Expr *rhs)
{
	Value l = interpret_expr(lhs);
	Value r = interpret_expr(rhs);

	if (l.value_type == Value_Number && r.value_type == Value_Number)
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
		if (l.value_type == r.value_type)                      \
		{                                                      \
			switch (l.value_type)                              \
			{                                                  \
				case Value_Number:                             \
					return value_bool(l.number op r.number);   \
				case Value_Bool:                               \
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
	if (left.value_type != Value_Bool)
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
	if (right.value_type != Value_Bool)
	{
		printf("Operands to `and` must be of type boolean\n");
		return value_bool(false);
	}

	return right;
}

static Value logic_or(Expr *lhs, Expr *rhs)
{
	Value left = interpret_expr(lhs);
	if (left.value_type != Value_Bool)
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
	if (right.value_type != Value_Bool)
	{
		printf("Operands to `or` must be of type boolean\n");
		return value_bool(false);
	}

	return right;
}

static Value interpret_expr(Expr *expr)
{
	switch (expr->expr_type)
	{
		case Expr_BooleanLiteral:
			return value_bool(expr->boolean);

		case Expr_NumberLiteral:
			return value_number(expr->number);

		case Expr_Grouping:
			return interpret_expr(expr->grouping.expr);

		case Expr_Binary: {
			switch (expr->binary.op)
			{
				case Token_Minus:
					return subtract(expr->binary.left, expr->binary.right);
				case Token_Plus:
					return add(expr->binary.left, expr->binary.right);
				case Token_Slash:
					return divide(expr->binary.left, expr->binary.right);
				case Token_Star:
					return mult(expr->binary.left, expr->binary.right);
				case Token_BangEqual:
					return neq(expr->binary.left, expr->binary.right);
				case Token_EqualEqual:
					return eq(expr->binary.left, expr->binary.right);
				case Token_Greater:
					return gt(expr->binary.left, expr->binary.right);
				case Token_GreaterEqual:
					return geq(expr->binary.left, expr->binary.right);
				case Token_Less:
					return lt(expr->binary.left, expr->binary.right);
				case Token_LessEqual:
					return leq(expr->binary.left, expr->binary.right);
				case Token_And:
					return logic_and(expr->binary.left, expr->binary.right);
				case Token_Or:
					return logic_or(expr->binary.left, expr->binary.right);

				default:
					UNREACHABLE();
			}
		}
		break;

		case Expr_Unary: {
			switch (expr->unary.op)
			{
				case Token_Minus: {
					Value v = interpret_expr(expr->unary.right);
					if (v.value_type == Value_Number)
					{
						return value_number(-v.number);
					}

					UNREACHABLE();
				}
				break;

				case Token_Not: {
					Value v = interpret_expr(expr->unary.right);
					if (v.value_type == Value_Bool)
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

		case Expr_Assignment: {
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

		case Expr_Identifier: {
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

		case Expr_Call: {
			Value callee = interpret_expr(expr->call.callee);

			assert(callee.value_type == Value_Function);

			frame_stack_push_frame(&frame_stack);

			int arity = darray_len(callee.function.args);
			assert(arity == darray_len(expr->call.arguments));

			for (int i = 0; i < arity; i++)
			{
				Identifier arg_name = callee.function.args[i];
				Value arg_value = interpret_expr(expr->call.arguments[i]);

				frame_stack_declare_variable(&frame_stack, arg_name, arg_value);
			}

			Result result = interpret_stmt(callee.function.body);

			frame_stack_pop_frame(&frame_stack);

			switch (result.result_type)
			{
				case Result_None:
					return value_nil();

				case Result_Return:
					return result.return_result.value;
			}
		}
		break;
	}

	UNREACHABLE();
}

static NODISCARD Result interpret_stmt(Stmt *stmt)
{
	switch (stmt->stmt_type)
	{
		case Stmt_Expr: {
			Value value = interpret_expr(stmt->expression.expr);
			UNUSED(value);
			return result_none();
		}
		break;

		case Stmt_Print: {
			Value value = interpret_expr(stmt->print.expr);

			switch (value.value_type)
			{
				case Value_Nil: {
					printf("<NIL>\n");
				}
				break;

				case Value_Bool: {
					printf("%s\n", (value.boolean ? "true" : "false"));
				}
				break;

				case Value_Number: {
					printf("%f\n", value.number);
				}
				break;

				default:
					UNREACHABLE();
			}

			return result_none();
		}
		break;

		case Stmt_VarDecl: {
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

		case Stmt_FunctionDecl: {
			Value value = value_function(stmt->function_decl.args,
										 stmt->function_decl.body);

			frame_stack_declare_variable(&frame_stack, stmt->function_decl.name,
										 value);

			return result_none();
		}
		break;

		case Stmt_Block: {
			frame_stack_push_frame(&frame_stack);

			Result block_result = result_none();

			int count = darray_len(stmt->block.statements);
			for (int i = 0; i < count; i++)
			{
				Result result = interpret_stmt(stmt->block.statements[i]);
				if (result.result_type == Result_Return)
				{
					block_result = result;
					break;
				}
			}

			frame_stack_pop_frame(&frame_stack);

			return block_result;
		}
		break;

		case Stmt_If: {
			Value value = interpret_expr(stmt->if_stmt.cond);
			if (value.value_type != Value_Bool)
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

		case Stmt_While: {
			Result block_result = result_none();

			while (true)
			{
				Value value = interpret_expr(stmt->while_stmt.cond);

				if (value.value_type != Value_Bool)
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
				if (result.result_type == Result_Return)
				{
					block_result = result;
					break;
				}
			}

			return block_result;
		}
		break;

		case Stmt_Return: {
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
