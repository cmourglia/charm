#include "lexer.h"

#include <string.h>

#include "token.h"

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

static Token token(Lexer *lexer, TokenType type);
static Token equal_token(Lexer *lexer, TokenType one_char, TokenType two_char);
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
			return token(lexer, TOKEN_EOF);

		case '(':
			return token(lexer, TOKEN_OPEN_PAREN);
		case ')':
			return token(lexer, TOKEN_CLOSE_PAREN);
		case '{':
			return token(lexer, TOKEN_OPEN_SQUIRLY);
		case '}':
			return token(lexer, TOKEN_CLOSE_SQUIRLY);
		case '[':
			return token(lexer, TOKEN_OPEN_BRACKET);
		case ']':
			return token(lexer, TOKEN_CLOSE_BRACKET);

		case ',':
			return token(lexer, TOKEN_COMMA);
		case '.':
			return token(lexer, TOKEN_DOT);
		case ';':
			return token(lexer, TOKEN_SEMICOLON);

		case '-':
			return token(lexer, TOKEN_MINUS);
		case '+':
			return token(lexer, TOKEN_PLUS);
		case '/':
			if (peek(lexer) == '/')
			{
				return comment_token(lexer);
			}
			return token(lexer, TOKEN_SLASH);
		case '*':
			return token(lexer, TOKEN_STAR);

		case '!':
			return equal_token(lexer, TOKEN_INVALID, TOKEN_BANG_EQUAL);
		case '=':
			return equal_token(lexer, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL);
		case '>':
			return equal_token(lexer, TOKEN_GREATER, TOKEN_GREATER_EQUAL);
		case '<':
			return equal_token(lexer, TOKEN_LESS, TOKEN_LESS_EQUAL);

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

			return token(lexer, TOKEN_INVALID);
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

static Token token(Lexer *lexer, TokenType type)
{
	Token token = {
		.type = type,
		.lexeme_start = lexer->start,
		.lexeme_len = (int)(lexer->current - lexer->start),
	};

	return token;
}

static Token equal_token(Lexer *lexer, TokenType one_char, TokenType two_char)
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
			return token(lexer, TOKEN_INVALID);
		}
		advance(lexer);
	}

	if (peek(lexer) != '"')
	{
		UNREACHABLE();
	}

	advance(lexer);

	lexer->start += 1;
	lexer->current -= 1;
	Token tk = token(lexer, TOKEN_STRING);
	lexer->current += 1;
	return tk;
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

	return token(lexer, TOKEN_NUMBER);
}

static Token check_keyword(Lexer *lexer, int start, int length,
						   const char *rest, TokenType type)
{
	if (lexer->current - lexer->start == start + length &&
		memcmp(lexer->start + start, rest, length) == 0)
	{
		return token(lexer, type);
	}
	return token(lexer, TOKEN_IDENTIFIER);
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
			return check_keyword(lexer, 1, 2, "nd", TOKEN_AND);
		case 'e':
			return check_keyword(lexer, 1, 3, "lse", TOKEN_ELSE);
		case 'i':
			return check_keyword(lexer, 1, 1, "f", TOKEN_IF);
		case 'n':
			return check_keyword(lexer, 1, 2, "ot", TOKEN_NOT);
		case 'o':
			return check_keyword(lexer, 1, 1, "r", TOKEN_OR);
		case 'r':
			return check_keyword(lexer, 1, 5, "eturn", TOKEN_RETURN);
		case 'v':
			return check_keyword(lexer, 1, 2, "ar", TOKEN_VAR);
		case 'w':
			return check_keyword(lexer, 1, 4, "hile", TOKEN_WHILE);

		case 'f':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 'a':
						return check_keyword(lexer, 2, 3, "lse", TOKEN_FALSE);
					case 'o':
						return check_keyword(lexer, 2, 1, "r", TOKEN_FOR);
					case 'u':
						return check_keyword(lexer, 2, 6, "nction",
											 TOKEN_FUNCTION);
				}
			}

		case 's':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 't':
						return check_keyword(lexer, 2, 4, "ruct", TOKEN_STRUCT);
					case 'u':
						return check_keyword(lexer, 2, 3, "per", TOKEN_SUPER);
				}
			}

		case 't':
			if (lexer->current - lexer->start > 1)
			{
				switch (lexer->start[1])
				{
					case 'h':
						return check_keyword(lexer, 2, 2, "is", TOKEN_THIS);
					case 'r':
						return check_keyword(lexer, 2, 2, "ue", TOKEN_TRUE);
				}
			}
	}

	return token(lexer, TOKEN_IDENTIFIER);
}

static Token comment_token(Lexer *lexer)
{
	while (peek(lexer) != '\n' && peek(lexer) != '\0')
	{
		advance(lexer);
	}

	return token(lexer, TOKEN_COMMENT);
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
