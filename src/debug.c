#include "debug.h"

#include "ast.h"
#include "common.h"
#include "parser.h"

#define GRY "\x1B[30m"
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

const char *debug_get_token_type_str(TokenType type)
{
	switch (type)
	{
		case Token_EOF:
			return "EOF";
		case Token_Invalid:
			return "Invalid";
		case Token_Comment:
			return "Comment";
		case Token_LeftParen:
			return "LeftParen";
		case Token_RightParen:
			return "RightParen";
		case Token_LeftBrace:
			return "LeftBrace";
		case Token_RightBrace:
			return "RightBrace";
		case Token_LeftBracket:
			return "LeftBracket";
		case Token_RightBracket:
			return "RightBracket";
		case Token_Comma:
			return "Comma";
		case Token_Dot:
			return "Dot";
		case Token_Semicolon:
			return "Semicolon";
		case Token_Minus:
			return "Minus";
		case Token_Plus:
			return "Plus";
		case Token_Slash:
			return "Slash";
		case Token_Star:
			return "Star";
		case Token_BangEqual:
			return "BangEqual";
		case Token_Equal:
			return "Equal";
		case Token_EqualEqual:
			return "EqualEqual";
		case Token_Greater:
			return "Greater";
		case Token_GreaterEqual:
			return "GreaterEqual";
		case Token_Less:
			return "Less";
		case Token_LessEqual:
			return "LessEqual";
		case Token_Identifier:
			return "Identifier";
		case Token_String:
			return "String";
		case Token_Number:
			return "Number";
		case Token_And:
			return "And";
		case Token_Else:
			return "Else";
		case Token_False:
			return "False";
		case Token_For:
			return "For";
		case Token_Function:
			return "Function";
		case Token_If:
			return "If";
		case Token_Nil:
			return "Nil";
		case Token_Not:
			return "Not";
		case Token_Or:
			return "Or";
		case Token_Return:
			return "Return";
		case Token_Struct:
			return "Struct";
		case Token_This:
			return "This";
		case Token_True:
			return "True";
		case Token_Var:
			return "Var";
		case Token_While:
			return "While";
		case Token_Super:
			return "Super";
		case Token_Print:
			return "Print";
	}

	UNREACHABLE();
}

int token_to_string(char *buffer, int capacity, Token token)
{
	return snprintf(buffer, capacity, "Token { %s: `%.*s` }",
					debug_get_token_type_str(token.type), token.lexeme_len,
					token.lexeme_start);
}

#define PRINT_EXPR_TYPE(t) printf(RED #t RESET "\n")
#define PRINT_EXPR_LITERAL(t, format, ...) \
	printf(RED #t RESET " <" CYN format RESET ">", __VA_ARGS__)

#define PRINT_EXPR_OP(tk)                                          \
	printf(GRN "%*sOp: " BLU "%s" RESET "\n", (level + 1) * 2, "", \
		   debug_get_token_type_str(tk))

#define PRINT_EXPR_CHILD(name, e)                                \
	do                                                           \
	{                                                            \
		printf(GRN "%*s" #name ": " RESET, (level + 1) * 2, ""); \
		print_expr(e, level + 1);                                \
	} while (false)

#define PRINT_STMT_TYPE(t) printf(YEL #t RESET "\n")

static void print_expr(Expr *expr, int level)
{
	switch (expr->expr_type)
	{
		case Expr_Binary: {
			PRINT_EXPR_TYPE(Binary expression);
			PRINT_EXPR_OP(expr->binary.op);
			PRINT_EXPR_CHILD(Left, expr->binary.left);
			printf("\n");
			PRINT_EXPR_CHILD(Right, expr->binary.right);
		}
		break;

		case Expr_Grouping: {
			PRINT_EXPR_TYPE(Grouping expression);
			printf("%*s", (level + 1) * 2, "");
			print_expr(expr->grouping.expr, level + 1);
		}
		break;

		case Expr_Unary: {
			PRINT_EXPR_TYPE(Unary Expression);
			PRINT_EXPR_OP(expr->unary.op);
			PRINT_EXPR_CHILD(Right, expr->unary.right);
		}
		break;

		case Expr_NumberLiteral: {
			PRINT_EXPR_LITERAL(Number, "%f", expr->number);
		}
		break;

		case Expr_BooleanLiteral: {
			PRINT_EXPR_LITERAL(Boolean, "%s", expr->boolean ? "true" : "false");
		}
		break;

		case Expr_Identifier: {
			PRINT_EXPR_LITERAL(Identifier, "%*s", expr->identifier.len,
							   expr->identifier.str);
		}
		break;
	}
}

static void print_stmt(Stmt *stmt, int level)
{
	switch (stmt->stmt_type)
	{
		case Stmt_Expr: {
			PRINT_STMT_TYPE(Expression statement);
			printf("%*s", (level + 1) * 2, "");
			print_expr(stmt->expression.expr, level + 1);
		}
		break;

		case Stmt_Print: {
			PRINT_STMT_TYPE(Print statement);
			printf("%*s", (level + 1) * 2, "");
			print_expr(stmt->expression.expr, level + 1);
		}
		break;

		case Stmt_VarDecl: {
			PRINT_STMT_TYPE(Variable Declaration);
			printf("%*s" GRN "Identifier: " BLU "%.*s" RESET, (level + 1) * 2,
				   "", stmt->var_decl.identifier.len,
				   stmt->var_decl.identifier.str);

			if (stmt->var_decl.expr != NULL)
			{
				printf("\n");
				printf("%*s" GRN "Expression: ", (level + 1) * 2, "");
				print_expr(stmt->var_decl.expr, level + 1);
			}
			//printf(RED #t RESET " <" GRY format RESET ">", (value))
		}
		break;
	}

	printf("\n");
}

void debug_print_program(Program program)
{
	for (int i = 0; i < program.statement_count; i++)
	{
		print_stmt(program.statements[i], 0);
	}
}
