#include "debug.h"

#include "core/common.h"
#include "core/value.h"

#include "ast/ast.h"

const char *debug_get_token_type_str(TokenType type)
{
	switch (type)
	{
		case TOKEN_EOF:
			return "EOF";
		case TOKEN_INVALID:
			return "Invalid";
		case TOKEN_COMMENT:
			return "Comment";
		case TOKEN_OPEN_PAREN:
			return "LeftParen";
		case TOKEN_CLOSE_PAREN:
			return "RightParen";
		case TOKEN_OPEN_SQUIRLY:
			return "LeftBrace";
		case TOKEN_CLOSE_SQUIRLY:
			return "RightBrace";
		case TOKEN_OPEN_BRACKET:
			return "LeftBracket";
		case TOKEN_CLOSE_BRACKET:
			return "RightBracket";
		case TOKEN_COMMA:
			return "Comma";
		case TOKEN_DOT:
			return "Dot";
		case TOKEN_SEMICOLON:
			return "Semicolon";
		case TOKEN_MINUS:
			return "Minus";
		case TOKEN_PLUS:
			return "Plus";
		case TOKEN_SLASH:
			return "Slash";
		case TOKEN_STAR:
			return "Star";
		case TOKEN_BANG_EQUAL:
			return "BangEqual";
		case TOKEN_EQUAL:
			return "Equal";
		case TOKEN_EQUAL_EQUAL:
			return "EqualEqual";
		case TOKEN_GREATER:
			return "Greater";
		case TOKEN_GREATER_EQUAL:
			return "GreaterEqual";
		case TOKEN_LESS:
			return "Less";
		case TOKEN_LESS_EQUAL:
			return "LessEqual";
		case TOKEN_IDENTIFIER:
			return "Identifier";
		case TOKEN_STRING:
			return "String";
		case TOKEN_NUMBER:
			return "Number";
		case TOKEN_AND:
			return "And";
		case TOKEN_ELSE:
			return "Else";
		case TOKEN_FALSE:
			return "False";
		case TOKEN_FOR:
			return "For";
		case TOKEN_FUNCTION:
			return "Function";
		case TOKEN_IF:
			return "If";
		case TOKEN_NIL:
			return "Nil";
		case TOKEN_NOT:
			return "Not";
		case TOKEN_OR:
			return "Or";
		case TOKEN_RETURN:
			return "Return";
		case TOKEN_STRUCT:
			return "Struct";
		case TOKEN_THIS:
			return "This";
		case TOKEN_TRUE:
			return "True";
		case TOKEN_VAR:
			return "Var";
		case TOKEN_WHILE:
			return "While";
		case TOKEN_SUPER:
			return "Super";
	}

	UNREACHABLE();
}

int token_to_string(char *buffer, int capacity, Token token)
{
	return snprintf(buffer, capacity, "Token { %s: `%.*s` }",
					debug_get_token_type_str(token.type), token.lexeme_len,
					token.lexeme_start);
}

void print_value(Value *value)
{
	switch (value->type)
	{
		case VALUE_NIL:
			printf("<NIL>");
			break;

		case VALUE_BOOL:
			printf("%s", (value->boolean ? "true" : "false"));
			break;

		case VALUE_NUMBER:
			printf("%f", value->number);
			break;

		case VALUE_STRING:
			printf("%.*s", value->string.len, value->string.str);
			break;

		default:
			UNREACHABLE();
	}
}
