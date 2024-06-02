#include "interpreter.h"

#include "hash_table.h"
#include "parser.h"

#include "ast.h"
#include "common.h"
#include "token.h"
#include "value.h"

static Value interpret_expr(Expr *expr);
static void interpret_stmt(Stmt *stmt);

static HashTable variables;

void interpreter_run(struct Program program)
{
	hash_table_init(&variables);

	int count = darray_len(program.statements);
	for (int i = 0; i < count; i++)
	{
		interpret_stmt(program.statements[i]);
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

		case Expr_Identifier: {
			Value value;
			hash_table_get(&variables, expr->identifier, &value);
			return value;
		}
		break;

		default:
			UNREACHABLE();
	}
}

static void interpret_stmt(Stmt *stmt)
{
	switch (stmt->stmt_type)
	{
		case Stmt_Expr: {
			Value value = interpret_expr(stmt->expression.expr);
			UNUSED(value);
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
		}
		break;

		case Stmt_VarDecl: {
			Value value = value_nil();

			if (stmt->var_decl.expr != NULL)
			{
				value = interpret_expr(stmt->var_decl.expr);
			}

			hash_table_set(&variables, stmt->var_decl.identifier, value);
		}
		break;

		case Stmt_Block: {
			int count = darray_len(stmt->block.statements);
			for (int i = 0; i < count; i++)
			{
				interpret_stmt(stmt->block.statements[i]);
			}
		}
		break;

		case Stmt_If: {
			Value value = interpret_expr(stmt->if_stmt.cond);
			if (value.value_type != Value_Bool)
			{
				printf("Error: if condition is not a boolean expression\n");
				return;
			}

			if (value.boolean)
			{
				interpret_stmt(stmt->if_stmt.then_branch);
			}
			else
			{
				if (stmt->if_stmt.else_branch != NULL)
				{
					interpret_stmt(stmt->if_stmt.else_branch);
				}
			}
		}
		break;
	}
}
