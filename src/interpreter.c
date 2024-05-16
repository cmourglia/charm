#include "interpreter.h"

#include <string.h>

#include "parser.h"

#include "ast.h"
#include "common.h"
#include "token.h"

typedef enum {
	Value_Number,
	Value_Bool,
	Value_String,
	Value_UserType,
} ValueType;

typedef struct {
	union {
		double number;
		bool boolean;
		// TODO: others
	};

	ValueType value_type;
} Value;

static Value value_number(double n)
{
	return (Value){
		.number = n,
		.value_type = Value_Number,
	};
}

static Value value_bool(bool b)
{
	return (Value){
		.boolean = b,
		.value_type = Value_Bool,
	};
}

static Value interpret_expr(Expr *expr);
static void interpret_stmt(Stmt *stmt);

void interpreter_run(struct Program program)
{
	for (int i = 0; i < program.statement_count; i++)
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
			// TODO
		}
		break;
	}
}
