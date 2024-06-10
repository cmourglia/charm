#include "debug.h"

#include <stdio.h>

#include "core/dyn_array.h"
#include "core/cell.h"

#include "ast/ast.h"

#include "debug/debug.h"

#define GRY "\x1B[30m"
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define PRINT_EXPR_TYPE(t) printf(RED #t RESET "\n")
#define PRINT_EXPR_LITERAL(t, format, ...) \
	printf(RED #t RESET " <" CYN format RESET ">", __VA_ARGS__)

#define PRINT_HEADER(name) \
	printf(GRN "%*.s" #name ": " RESET, (level + 1) * 2, "")

#define PRINT_IDENTIFIER(color, identifier) \
	do                                      \
	{                                       \
		printf(color);                      \
		print_cell((Cell *)identifier);     \
		printf(RESET);                      \
	} while (false)

#define PRINT_EXPR_OP(tk)                                          \
	do                                                             \
	{                                                              \
		PRINT_HEADER(Op);                                          \
		printf(BLU "%s" RESET "\n", debug_get_token_type_str(tk)); \
	} while (false)

#define PRINT_EXPR_CHILD(name, e)                                 \
	do                                                            \
	{                                                             \
		printf(GRN "%*.s" #name ": " RESET, (level + 1) * 2, ""); \
		print_expr(e, level + 1);                                 \
	} while (false)

#define PRINT_STMT_TYPE(t) printf(YEL #t RESET "\n")

#define PRINT_STMT_CHILD(name) \
	printf("%*.s" GRN #name ": " RESET, (level + 1) * 2, "");

#define INDENT() printf("%*.s", (level + 1) * 2, "")

static void print_expr(Expr *expr, int level)
{
	switch (expr->type)
	{
		case EXPR_BINARY:
		{
			PRINT_EXPR_TYPE(Binary expression);
			PRINT_EXPR_OP(expr->as.binary.op);
			PRINT_EXPR_CHILD(Left, expr->as.binary.left);
			printf("\n");
			PRINT_EXPR_CHILD(Right, expr->as.binary.right);
		}
		break;

		case EXPR_GROUPING:
		{
			PRINT_EXPR_TYPE(Grouping expression);
			INDENT();
			print_expr(expr->as.grouping.expr, level + 1);
		}
		break;

		case EXPR_UNARY:
		{
			PRINT_EXPR_TYPE(Unary Expression);
			PRINT_EXPR_OP(expr->as.unary.op);
			PRINT_EXPR_CHILD(Right, expr->as.unary.right);
		}
		break;

		case EXPR_ASSIGNMENT:
		{
			PRINT_EXPR_TYPE(Assignment);
			PRINT_HEADER(Name);
			PRINT_IDENTIFIER(BLU, expr->as.assignment.name);
			printf("\n");
			PRINT_EXPR_CHILD(Value, expr->as.assignment.value);
		}
		break;

		case EXPR_CALL:
		{
			PRINT_EXPR_TYPE(Call);
			PRINT_EXPR_CHILD(Callee, expr->as.call.callee);
			if (darray_len(expr->as.call.arguments) > 0)
			{
				printf("\n");
				PRINT_HEADER(Arguments);
				level += 1;
				for (int i = 0; i < darray_len(expr->as.call.arguments); i++)
				{
					printf("\n");
					INDENT();
					print_expr(expr->as.call.arguments[i], level + 1);
				}
				level -= 1;
			}
		}
		break;

		case EXPR_NUMBER_LITERAL:
		{
			PRINT_EXPR_LITERAL(Number, "%f", expr->as.number);
		}
		break;

		case EXPR_BOOLEAN_LITERAL:
		{
			PRINT_EXPR_LITERAL(Boolean, "%s",
							   expr->as.boolean ? "true" : "false");
		}
		break;

		case EXPR_CELL_LITERAL:
		{
			PRINT_EXPR_LITERAL(Cell, "%p", expr->as.cell);
			// TODO: Print more info about the cell at some point
		}
		break;

		case EXPR_IDENTIFIER:
		{
			PRINT_EXPR_LITERAL(Identifier, "%s", expr->as.identifier->str);
		}
		break;
	}
}

static void print_stmt(Stmt *stmt, int level)
{
	switch (stmt->type)
	{
		case STMT_EXPR:
		{
			PRINT_STMT_TYPE(Expression statement);
			INDENT();
			print_expr(stmt->as.expression.expr, level + 1);
			printf("\n");
		}
		break;

		case STMT_VAR_DECL:
		{
			PRINT_STMT_TYPE(Variable Declaration);
			PRINT_HEADER(Name);
			PRINT_IDENTIFIER(BLU, stmt->as.var_decl.name);

			if (stmt->as.var_decl.expr != NULL)
			{
				printf("\n");
				printf("%*.s" GRN "Expression: ", (level + 1) * 2, "");
				print_expr(stmt->as.var_decl.expr, level + 1);
			}

			printf("\n");
			//printf(RED #t RESET " <" GRY format RESET ">", (value))
		}
		break;

		case STMT_FUNCTION_DECL:
		{
			PRINT_STMT_TYPE(Function Declaration);
			PRINT_HEADER(Name);
			PRINT_IDENTIFIER(BLU, stmt->as.function_decl.name);
			printf("\n");

			int arity = darray_len(stmt->as.function_decl.args);
			PRINT_HEADER(Arity);
			printf("%d\n", arity);

			if (arity > 0)
			{
				PRINT_HEADER(Args);
				for (int i = 0; i < arity; i++)
				{
					if (i != 0)
					{
						printf(", ");
					}

					PRINT_IDENTIFIER(BLU, stmt->as.function_decl.args[i]);
				}

				printf("\n");
			}

			PRINT_HEADER(Body);
			print_stmt(stmt->as.function_decl.body, level + 1);
		}
		break;

		case STMT_BLOCK:
		{
			PRINT_STMT_TYPE(Block);
			int count = darray_len(stmt->as.block.statements);
			for (int i = 0; i < count; i++)
			{
				INDENT();
				print_stmt(stmt->as.block.statements[i], level + 1);
			}
		}
		break;

		case STMT_IF:
		{
			PRINT_STMT_TYPE(If);
			PRINT_STMT_CHILD(Condition);
			print_expr(stmt->as.if_stmt.cond, level + 1);
			printf("\n");
			PRINT_STMT_CHILD(Then);
			print_stmt(stmt->as.if_stmt.then_branch, level + 1);
			if (stmt->as.if_stmt.else_branch != NULL)
			{
				PRINT_STMT_CHILD(Else);
				print_stmt(stmt->as.if_stmt.else_branch, level + 1);
			}
		}
		break;

		case STMT_WHILE:
		{
			PRINT_STMT_TYPE(While);
			PRINT_STMT_CHILD(Condition);
			print_expr(stmt->as.while_stmt.cond, level + 1);
			printf("\n");
			PRINT_STMT_CHILD(Body);
			print_stmt(stmt->as.while_stmt.body, level + 1);
		}
		break;

		case STMT_RETURN:
		{
			PRINT_STMT_TYPE(Return);
			PRINT_STMT_CHILD(Expression);
			if (stmt->as.return_stmt.expr == NULL)
			{
				printf("<NONE>");
			}
			else
			{
				print_expr(stmt->as.return_stmt.expr, level + 1);
			}

			printf("\n");
		}
	}
}

void debug_print_program(Program program)
{
	int count = darray_len(program.statements);
	for (int i = 0; i < count; i++)
	{
		print_stmt(program.statements[i], 0);
	}
	printf("\n");
}
