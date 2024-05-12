#include "parser.h"

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "lexer.h"
#include "ast.h"
#include "token.h"

Parser parser_init(struct Lexer *lexer)
{
	Parser parser = {
		.lexer = lexer,
	};

	return parser;
}

static void consume_token(Parser *parser);
static Token advance(Parser *parser);
static bool match(Parser *parser, TokenType type);

// expression -> equality ;
// equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term       -> factor ( ( "-" | "+" ) factor )* ;
// factor     -> unary ( ( "/" | "*" ) unary )* ;
// unary      -> ("not" | "-") unary
//             | primary ;
// primary    -> NUMBER | STRING | "true" | "false" | "nil"
//             | "(" expression ")" ;
static Expr *expression(Parser *parser);
static Expr *equality(Parser *parser);
static Expr *comparison(Parser *parser);
static Expr *term(Parser *parser);
static Expr *factor(Parser *parser);
static Expr *unary(Parser *parser);
static Expr *primary(Parser *parser);

struct Expr *parser_parse_program(Parser *parser)
{
	parser->curr_token = lexer_get_next_token(parser->lexer);
	return expression(parser);

	//	UNUSED(parser);
	//
	//	Expr *x = ast_expr_number_literal(123);
	//	Expr *y = ast_expr_boolean_literal(false);
	//	Expr *u = ast_expr_unary(Token_Minus, x);
	//	Expr *g = ast_expr_grouping(y);
	//	Expr *b = ast_expr_binary(Token_Star, u, g);
	//
	//	return b;
}

// expression -> equality;
static Expr *expression(Parser *parser)
{
	return equality(parser);
}

// equality -> comparison ( ( "!=" | "==" ) comparison )* ;
static Expr *equality(Parser *parser)
{
	Expr *expr = comparison(parser);

	while (match(parser, Token_BangEqual) || match(parser, Token_EqualEqual))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = comparison(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )? ;
static Expr *comparison(Parser *parser)
{
	Expr *expr = term(parser);

	while (match(parser, Token_Greater) || match(parser, Token_GreaterEqual) ||
		   match(parser, Token_Less) || match(parser, Token_LessEqual))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = term(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

// term       -> factor ( ( "-" | "+" ) factor )* ;
static Expr *term(Parser *parser)
{
	Expr *expr = factor(parser);

	while (match(parser, Token_Minus) || match(parser, Token_Plus))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = factor(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

// factor     -> unary ( ( "/" | "*" ) unary )* ;
static Expr *factor(Parser *parser)
{
	Expr *expr = unary(parser);

	while (match(parser, Token_Slash) || match(parser, Token_Star))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = unary(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

// unary      -> ("not" | "-") unary
//             | primary ;
static Expr *unary(Parser *parser)
{
	if (match(parser, Token_Not) || match(parser, Token_Minus))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = unary(parser);

		return ast_expr_unary(op, right);
	}

	return primary(parser);
}

// primary    -> NUMBER | STRING | "true" | "false" | "nil"
//             | "(" expression ")" ;
static Expr *primary(Parser *parser)
{
	switch (parser->curr_token.type)
	{
		case Token_Number: {
			Token tk = advance(parser);

			char buf[512];
			memcpy(buf, tk.lexeme_start, tk.lexeme_len);
			buf[tk.lexeme_len] = '\0';
			double value = atof(buf);

			return ast_expr_number_literal(value);
		}
		break;

		case Token_String: {
			// TODO
			UNREACHABLE();
		}
		break;

		case Token_True:
			return ast_expr_boolean_literal(true);

		case Token_False: {
			return ast_expr_boolean_literal(false);
		}

		case Token_LeftParen: {
			consume_token(parser);

			Expr *expr = expression(parser);

			if (!match(parser, Token_RightParen))
			{
				// TODO: Handle errors
				UNREACHABLE();
			}

			return ast_expr_grouping(expr);
		}
		break;

		default:
			UNREACHABLE();
	}
}

static bool match(Parser *parser, TokenType type)
{
	if (parser->curr_token.type == type)
	{
		advance(parser);
		return true;
	}

	return false;
}

static void consume_token(Parser *parser)
{
	parser->prev_token = parser->curr_token;
	parser->curr_token = lexer_get_next_token(parser->lexer);
}

static Token advance(Parser *parser)
{
	if (parser->curr_token.type != Token_EOF)
	{
		consume_token(parser);
	}

	return parser->prev_token;
}
