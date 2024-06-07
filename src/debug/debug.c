#include "debug.h"

#include "core/common.h"

const char *debug_get_token_type_str(Token_Type type)
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
	}

	UNREACHABLE();
}

int token_to_string(char *buffer, int capacity, Token token)
{
	return snprintf(buffer, capacity, "Token { %s: `%.*s` }",
					debug_get_token_type_str(token.type), token.lexeme_len,
					token.lexeme_start);
}
