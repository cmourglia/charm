#include "lexer.h"

#include "common.h"
#include "token.h"

#include <string.h>

Lexer lexer_init(const char *source)
{
	Lexer lexer = {
		.input = source,
		.start = source,
		.current = source,
	};

	return lexer;
}

void lexer_free(Lexer *lexer)
{
	(void)lexer;
}

static char advance(Lexer *lexer);
static char peek(Lexer *lexer);
static char peek_next(Lexer *lexer);

static Token token(Lexer *lexer, Token_Type type);
static Token equal_token(Lexer *lexer, Token_Type one_char,
						 Token_Type two_char);
static Token string_token(Lexer *lexer);
static Token number_token(Lexer *lexer);
static Token identifier_token(Lexer *lexer);
static Token comment_token(Lexer *lexer);

static bool is_whitespace(char c);
static bool is_alpha(char c);
static bool is_digit(char c);
static bool is_alphanum(char c);

Token lexer_get_next_token(Lexer *lexer)
{
	for (; is_whitespace(peek(lexer)); advance(lexer))
	{
	}

	lexer->start = lexer->current;

	char c = advance(lexer);

	switch (c)
	{
		case '\0':
			return token(lexer, Token_EOF);

		case '(':
			return token(lexer, Token_LeftParen);
		case ')':
			return token(lexer, Token_RightParen);
		case '{':
			return token(lexer, Token_LeftBrace);
		case '}':
			return token(lexer, Token_RightBrace);
		case '[':
			return token(lexer, Token_LeftBracket);
		case ']':
			return token(lexer, Token_RightBracket);

		case ',':
			return token(lexer, Token_Comma);
		case '.':
			return token(lexer, Token_Dot);
		case ';':
			return token(lexer, Token_Semicolon);

		case '-':
			return token(lexer, Token_Minus);
		case '+':
			return token(lexer, Token_Plus);
		case '/':
			if (peek(lexer) == '/')
			{
				return comment_token(lexer);
			}
			return token(lexer, Token_Slash);
		case '*':
			return token(lexer, Token_Star);

		case '!':
			return equal_token(lexer, Token_Invalid, Token_BangEqual);
		case '=':
			return equal_token(lexer, Token_Equal, Token_EqualEqual);
		case '>':
			return equal_token(lexer, Token_Greater, Token_GreaterEqual);
		case '<':
			return equal_token(lexer, Token_Less, Token_LessEqual);

		case '"':
			return string_token(lexer);

		default:
			if (is_digit(c))
			{
				return number_token(lexer);
			}

			if (is_alpha(c))
			{
				return identifier_token(lexer);
			}

			return token(lexer, Token_Invalid);
	}
}

static char advance(Lexer *lexer)
{
	return *lexer->current++;
}

static char peek(Lexer *lexer)
{
	return lexer->current[0];
}

static char peek_next(Lexer *lexer)
{
	return lexer->current[1];
}

static Token token(Lexer *lexer, Token_Type type)
{
	Token token = {
		.type = type,
		.lexeme_start = lexer->start,
		.lexeme_len = (int)(lexer->current - lexer->start),
	};

	return token;
}

static Token equal_token(Lexer *lexer, Token_Type one_char, Token_Type two_char)
{
	if (peek(lexer) == '=')
	{
		advance(lexer);
		return token(lexer, two_char);
	}

	return token(lexer, one_char);
}

static Token string_token(Lexer *lexer)
{
	advance(lexer);

	while (peek(lexer) != '"')
	{
		if (peek(lexer) == '\0')
		{
			return token(lexer, Token_Invalid);
		}
		advance(lexer);
	}

	if (peek(lexer) != '"')
	{
		UNREACHABLE();
	}

	advance(lexer);

	return token(lexer, Token_String);
}

static Token number_token(Lexer *lexer)
{
	while (is_digit(peek(lexer)))
	{
		advance(lexer);
	}

	if (peek(lexer) == '.' && is_digit(peek_next(lexer)))
	{
		advance(lexer);

		while (is_digit(peek(lexer)))
		{
			advance(lexer);
		}
	}

	return token(lexer, Token_Number);
}

static Token check_keyword(Lexer *lexer, int start, int length,
						   const char *rest, Token_Type type)
{
	if (lexer->current - lexer->start == start + length &&
		memcmp(lexer->start + start, rest, length) == 0)
	{
		return token(lexer, type);
	}
	return token(lexer, Token_Identifier);
}

static Token identifier_token(Lexer *lexer)
{
	while (is_alphanum(peek(lexer)))
	{
		advance(lexer);
	}

	switch (lexer->start[0])
	{
		case 'a':
			return check_keyword(lexer, 1, 2, "nd", Token_And);
		case 'e':
			return check_keyword(lexer, 1, 3, "lse", Token_Else);
		case 'i':
			return check_keyword(lexer, 1, 1, "f", Token_If);
		case 'n':
			return check_keyword(lexer, 1, 2, "ot", Token_Not);
		case 'o':
			return check_keyword(lexer, 1, 1, "r", Token_Or);
		case 'r':
			return check_keyword(lexer, 1, 5, "eturn", Token_Return);
		case 'v':
			return check_keyword(lexer, 1, 2, "ar", Token_Var);
		case 'w':
			return check_keyword(lexer, 1, 4, "hile", Token_While);

		case 'f':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 'a':
						return check_keyword(lexer, 2, 3, "lse", Token_False);
					case 'o':
						return check_keyword(lexer, 2, 1, "r", Token_For);
					case 'u':
						return check_keyword(lexer, 2, 6, "nction",
											 Token_Function);
				}
			}

		case 's':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 't':
						return check_keyword(lexer, 2, 4, "ruct", Token_Struct);
					case 'u':
						return check_keyword(lexer, 2, 3, "per", Token_Super);
				}
			}

		case 't':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 'h':
						return check_keyword(lexer, 2, 2, "is", Token_This);
					case 'r':
						return check_keyword(lexer, 2, 2, "ue", Token_True);
				}
			}
	}

	return token(lexer, Token_Identifier);
}

static Token comment_token(Lexer *lexer)
{
	while (peek(lexer) != '\n' && peek(lexer) != '\0')
	{
		advance(lexer);
	}

	return token(lexer, Token_Comment);
}

static bool is_whitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static bool is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}
static bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}
static bool is_alphanum(char c)
{
	return is_alpha(c) || is_digit(c);
}
