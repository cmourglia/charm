#include "parser.h"

#include "beard_lib.h"
#include "debug.h"
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

	hash_table_init(&parser.identifiers);

	return parser;
}

static Token advance(Parser *parser);
static Token consume(Parser *parser, TokenType expected);
static bool match(Parser *parser, TokenType type);

// program      -> declaration* EOF ;
// declaration  -> var_decl | statement
// statement    -> expr_stmt | print_stmt | if_stmt | block_stmt;
// expr_stmt    -> expression ";" ;
// print_stmt   -> "print" expression ";" ;
// var_decl     -> "var" IDENTIFIER ( "=" expression )? ";" ;
// if_stmt      -> "if" expression block_stmt ( "else" if_stmt | block_stmt ) ? ;
// block_stmt   -> "{" ( statement )* "}" ;
static Stmt *declaration(Parser *parser);
static Stmt *statement(Parser *parser);
static Stmt *expr_stmt(Parser *parser);
static Stmt *print_stmt(Parser *parser);
static Stmt *var_decl(Parser *parser);
static Stmt *if_stmt(Parser *parser);
static Stmt *block_stmt(Parser *parser);

// expression   -> logic_or ;
// logic_or     -> logic_and ( "or" logic_and )* ;
// logic_and    -> equality ( "and" equality )* ;
// equality     -> comparison ( ( "!=" | "==" ) comparison )* ;
// comparison   -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term         -> factor ( ( "-" | "+" ) factor )* ;
// factor       -> unary ( ( "/" | "*" ) unary )* ;
// unary        -> ("not" | "-") unary
//               | primary ;
// primary      -> NUMBER | STRING | "true" | "false" | "nil"
//               | "(" expression ")" | IDENTIFIER;
static Expr *expression(Parser *parser);
static Expr *logic_or(Parser *parser);
static Expr *logic_and(Parser *parser);
static Expr *equality(Parser *parser);
static Expr *comparison(Parser *parser);
static Expr *term(Parser *parser);
static Expr *factor(Parser *parser);
static Expr *unary(Parser *parser);
static Expr *primary(Parser *parser);

static void append_stmt(Program *program, Stmt *stmt);

struct Program parser_parse_program(Parser *parser)
{
	Program program = { 0 };

	parser->curr_token = lexer_get_next_token(parser->lexer);

	while (parser->curr_token.type != Token_EOF)
	{
		append_stmt(&program, declaration(parser));
	}

	return program;
}

static void append_stmt(Program *program, Stmt *stmt)
{
	// NOTE: clang-tidy complains about sizeof(Stmt*), which is fine here
	// NOLINTNEXTLINE
	darray_push(program->statements, stmt);
}

// expression -> equality;
static Expr *expression(Parser *parser)
{
	return logic_or(parser);
}

// logic_or     -> logic_and ( "or" logic_and )* ;
static Expr *logic_or(Parser *parser)
{
	Expr *expr = logic_and(parser);

	while (match(parser, Token_Or))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = logic_and(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

// logic_and    -> equality ( "and" equality )* ;
static Expr *logic_and(Parser *parser)
{
	Expr *expr = equality(parser);

	while (match(parser, Token_And))
	{
		TokenType op = parser->prev_token.type;
		Expr *right = equality(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
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
//             | "(" expression ")" | IDENTIFIER ;
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

		case Token_True: {
			advance(parser);
			return ast_expr_boolean_literal(true);
		}

		case Token_False: {
			advance(parser);
			return ast_expr_boolean_literal(false);
		}

		case Token_LeftParen: {
			advance(parser);

			Expr *expr = expression(parser);

			if (!match(parser, Token_RightParen))
			{
				// TODO: Handle errors
				UNREACHABLE();
			}

			return ast_expr_grouping(expr);
		}
		break;

		case Token_Identifier: {
			Token tk = advance(parser);
			Identifier identifier = ast_identifier(&parser->identifiers, tk);
			return ast_expr_identifier(identifier);
		}
		break;

		default:
			UNREACHABLE();
	}
}

// declaration -> var_decl | statement ;
static Stmt *declaration(Parser *parser)
{
	if (match(parser, Token_Var))
	{
		return var_decl(parser);
	}

	return statement(parser);
}

// statement  -> expr_stmt | print_stmt | if_stmt | block_stmt;
static Stmt *statement(Parser *parser)
{
	if (match(parser, Token_If))
	{
		return if_stmt(parser);
	}

	if (match(parser, Token_LeftBrace))
	{
		return block_stmt(parser);
	}

	if (match(parser, Token_Print))
	{
		return print_stmt(parser);
	}

	return expr_stmt(parser);
}

// expr_stmt  -> expression ";" ;
static Stmt *expr_stmt(Parser *parser)
{
	Expr *expr = expression(parser);

	consume(parser, Token_Semicolon);

	return ast_stmt_expression(expr);
}

// print_stmt -> "print" expression ";" ;
static Stmt *print_stmt(Parser *parser)
{
	Expr *expr = expression(parser);

	consume(parser, Token_Semicolon);

	return ast_stmt_print(expr);
}

// var_decl     -> "var" IDENTIFIER ( "=" expression )? ";" ;
static Stmt *var_decl(Parser *parser)
{
	Token identifier_token = consume(parser, Token_Identifier);
	Identifier identifier = ast_identifier(&parser->identifiers,
										   identifier_token);

	Expr *expr = NULL;

	if (match(parser, Token_Equal))
	{
		expr = expression(parser);
	}

	consume(parser, Token_Semicolon);

	return ast_stmt_var_decl(identifier, expr);
}

// if_stmt      -> "if" expression block_stmt ( "else" block_stmt | if_stmt) ? ;
static Stmt *if_stmt(Parser *parser)
{
	Expr *cond = expression(parser);

	consume(parser, Token_LeftBrace);
	Stmt *then_branch = block_stmt(parser);

	Stmt *else_branch = NULL;
	if (match(parser, Token_Else))
	{
		if (match(parser, Token_If))
		{
			else_branch = if_stmt(parser);
		}
		else
		{
			consume(parser, Token_LeftBrace);
			else_branch = block_stmt(parser);
		}
	}

	return ast_stmt_if(cond, then_branch, else_branch);
}

// block_stmt   -> "{" ( declaration )* "}" ;
static Stmt *block_stmt(Parser *parser)
{
	Stmt **statements = NULL;
	while (parser->curr_token.type != Token_EOF &&
		   parser->curr_token.type != Token_RightBrace)
	{
		// NOLINTNEXTLINE
		darray_push(statements, declaration(parser));
	}

	consume(parser, Token_RightBrace);

	return ast_stmt_block(statements);
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

// FIXME: Find a better name
static Token consume(Parser *parser, TokenType expected)
{
	if (parser->curr_token.type == expected)
	{
		return advance(parser);
	}

	printf("Expected token type `%s`, found `%s`\n",
		   debug_get_token_type_str(expected),
		   debug_get_token_type_str(parser->curr_token.type));
	UNREACHABLE();
}

static Token advance(Parser *parser)
{
	if (parser->curr_token.type != Token_EOF)
	{
		parser->prev_token = parser->curr_token;
		parser->curr_token = lexer_get_next_token(parser->lexer);
	}

	return parser->prev_token;
}