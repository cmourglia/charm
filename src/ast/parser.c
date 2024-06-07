#include "parser.h"

#include <string.h>
#include <stdlib.h>

#include "core/common.h"
#include "core/dyn_array.h"

#include "lexer.h"
#include "ast.h"
#include "token.h"

#include "debug/debug.h"

Parser parser_init(struct Lexer *lexer)
{
	Parser parser = {
		.lexer = lexer,
	};

	hash_table_init(&parser.identifiers);

	return parser;
}

static Token advance(Parser *parser);
static Token consume(Parser *parser, Token_Type expected);
static bool match(Parser *parser, Token_Type type);
static bool check(Parser *parser, Token_Type type);

// program          -> declaration* EOF ;
// declaration      -> var_decl | fun_decl | statement
// statement        -> expr_stmt | print_stmt | if_stmt | block_stmt
//                   | while_stmt | for_stmt | return_stmt ;
// expr_stmt        -> expression ";" ;
// var_decl         -> "var" IDENTIFIER ( "=" expression )? ";" ;
// function_decl    -> "function" function ;
// function         -> IDENTIFIER "(" parameters? ")" block_stmt ;
// if_stmt          -> "if" expression block_stmt
//                     ( "else" if_stmt | block_stmt ) ? ;
// block_stmt       -> "{" ( statement )* "}" ;
// while_stmt       -> "while" expression block_stmt ;
// for_stmt         -> "for" ( var_decl | expr_stmt | ";" )
//                     expression? ";"
//                     expression? block_stmt ;
// return_stmt      -> "return" expression? ";" ;
static Stmt *declaration(Parser *parser);
static Stmt *statement(Parser *parser);
static Stmt *expr_stmt(Parser *parser);
static Stmt *var_decl(Parser *parser);
static Stmt *function(Parser *parser);
static Stmt *if_stmt(Parser *parser);
static Stmt *block_stmt(Parser *parser);
static Stmt *while_stmt(Parser *parser);
static Stmt *for_stmt(Parser *parser);
static Stmt *return_stmt(Parser *parser);

// expression   -> assignment ;
// assignment   -> IDENTIFIER "=" assignment | logic_or ;
// logic_or     -> logic_and ( "or" logic_and )* ;
// logic_and    -> equality ( "and" equality )* ;
// equality     -> comparison ( ( "!=" | "==" ) comparison )* ;
// comparison   -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term         -> factor ( ( "-" | "+" ) factor )* ;
// factor       -> unary ( ( "/" | "*" ) unary )* ;
// unary        -> ("not" | "-") unary
//               | call ;
// call         -> primary ( "(" arguments? ")" "* ;
// arguments    -> expression ( "," expression )* ;
// primary      -> NUMBER | STRING | "true" | "false" | "nil"
//               | "(" expression ")" | IDENTIFIER;
static Expr *expression(Parser *parser);
static Expr *assignment(Parser *parser);
static Expr *logic_or(Parser *parser);
static Expr *logic_and(Parser *parser);
static Expr *equality(Parser *parser);
static Expr *comparison(Parser *parser);
static Expr *term(Parser *parser);
static Expr *factor(Parser *parser);
static Expr *unary(Parser *parser);
static Expr *call(Parser *parser);
static Expr *primary(Parser *parser);

static void append_stmt(Program *program, Stmt *stmt);

struct Program parser_parse_program(Parser *parser)
{
	Program program = { 0 };

	parser->curr_token = lexer_get_next_token(parser->lexer);

	while (parser->curr_token.type != Token_EOF)
	{
		Stmt *stmt = declaration(parser);
		if (stmt != NULL)
		{
			append_stmt(&program, stmt);
		}
	}

	return program;
}

static void append_stmt(Program *program, Stmt *stmt)
{
	// NOTE: clang-tidy complains about sizeof(Stmt*), which is fine here
	// NOLINTNEXTLINE(bugprone-sizeof-expression)
	darray_push(program->statements, stmt);
}

static Expr *expression(Parser *parser)
{
	return assignment(parser);
}

static Expr *assignment(Parser *parser)
{
	Expr *expr = logic_or(parser);

	if (match(parser, Token_Equal))
	{
		Expr *value = logic_or(parser);

		if (expr->expr_type == Expr_Identifier)
		{
			Identifier name = expr->identifier;
			free(expr);

			return ast_expr_assignment(name, value);
		}

		UNREACHABLE();
	}

	return expr;
}

static Expr *logic_or(Parser *parser)
{
	Expr *expr = logic_and(parser);

	while (match(parser, Token_Or))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = logic_and(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *logic_and(Parser *parser)
{
	Expr *expr = equality(parser);

	while (match(parser, Token_And))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = equality(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *equality(Parser *parser)
{
	Expr *expr = comparison(parser);

	while (match(parser, Token_BangEqual) || match(parser, Token_EqualEqual))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = comparison(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *comparison(Parser *parser)
{
	Expr *expr = term(parser);

	while (match(parser, Token_Greater) || match(parser, Token_GreaterEqual) ||
		   match(parser, Token_Less) || match(parser, Token_LessEqual))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = term(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *term(Parser *parser)
{
	Expr *expr = factor(parser);

	while (match(parser, Token_Minus) || match(parser, Token_Plus))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = factor(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *factor(Parser *parser)
{
	Expr *expr = unary(parser);

	while (match(parser, Token_Slash) || match(parser, Token_Star))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = unary(parser);

		expr = ast_expr_binary(op, expr, right);
	}

	return expr;
}

static Expr *unary(Parser *parser)
{
	if (match(parser, Token_Not) || match(parser, Token_Minus))
	{
		Token_Type op = parser->prev_token.type;
		Expr *right = unary(parser);

		return ast_expr_unary(op, right);
	}

	return call(parser);
}

static Expr *finish_call(Parser *parser, Expr *expr);

static Expr *call(Parser *parser)
{
	Expr *expr = primary(parser);

	while (true)
	{
		if (match(parser, Token_LeftParen))
		{
			expr = finish_call(parser, expr);
		}
		else
		{
			break;
		}
	}

	return expr;
}

static Expr *finish_call(Parser *parser, Expr *callee)
{
	Expr **arguments = NULL;

	if (!check(parser, Token_RightParen))
	{
		do
		{
			// FIXME: Limit the number of arguments ?

			// NOLINTNEXTLINE(bugprone-sizeof-expression)
			darray_push(arguments, expression(parser));
		} while (match(parser, Token_Comma));
	}

	consume(parser, Token_RightParen);

	return ast_expr_call(callee, arguments);
}

static Expr *primary(Parser *parser)
{
	switch (parser->curr_token.type)
	{
		case Token_Number:
		{
			Token tk = advance(parser);

			char buf[512];
			memcpy(buf, tk.lexeme_start, tk.lexeme_len);
			buf[tk.lexeme_len] = '\0';
			double value = atof(buf);

			return ast_expr_number_literal(value);
		}
		break;

		case Token_String:
		{
			// TODO
			UNREACHABLE();
		}
		break;

		case Token_True:
		{
			advance(parser);
			return ast_expr_boolean_literal(true);
		}

		case Token_False:
		{
			advance(parser);
			return ast_expr_boolean_literal(false);
		}

		case Token_LeftParen:
		{
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

		case Token_Identifier:
		{
			Token tk = advance(parser);
			Identifier identifier = ast_identifier(&parser->identifiers, tk);
			return ast_expr_identifier(identifier);
		}
		break;

		default:
			printf("Unexpected token %s\n",
				   debug_get_token_type_str(parser->curr_token.type));
			UNREACHABLE();
	}
}

static Stmt *declaration(Parser *parser)
{
	while (match(parser, Token_Comment))
	{
		// Skip comments
	}

	if (check(parser, Token_EOF))
	{
		return NULL;
	}

	if (match(parser, Token_Var))
	{
		return var_decl(parser);
	}

	if (match(parser, Token_Function))
	{
		return function(parser);
	}

	return statement(parser);
}

static Stmt *statement(Parser *parser)
{
	if (match(parser, Token_If))
	{
		return if_stmt(parser);
	}

	if (match(parser, Token_While))
	{
		return while_stmt(parser);
	}

	if (match(parser, Token_For))
	{
		return for_stmt(parser);
	}

	if (match(parser, Token_LeftBrace))
	{
		return block_stmt(parser);
	}

	if (match(parser, Token_Return))
	{
		return return_stmt(parser);
	}

	return expr_stmt(parser);
}

static Stmt *expr_stmt(Parser *parser)
{
	Expr *expr = expression(parser);

	consume(parser, Token_Semicolon);

	return ast_stmt_expression(expr);
}

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

static Stmt *function(Parser *parser)
{
	Token name_token = consume(parser, Token_Identifier);
	Identifier name = ast_identifier(&parser->identifiers, name_token);

	consume(parser, Token_LeftParen);

	Identifier *args = NULL;

	if (!check(parser, Token_RightParen))
	{
		do
		{
			Token arg_token = consume(parser, Token_Identifier);
			darray_push(args, ast_identifier(&parser->identifiers, arg_token));
		} while (match(parser, Token_Comma));
	}

	consume(parser, Token_RightParen);

	// Read args
	consume(parser, Token_LeftBrace);
	Stmt *body = block_stmt(parser);

	return ast_stmt_function_decl(name, args, body);
}

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

static Stmt *block_stmt(Parser *parser)
{
	Stmt **statements = NULL;
	while (parser->curr_token.type != Token_EOF &&
		   parser->curr_token.type != Token_RightBrace)
	{
		// NOLINTNEXTLINE(bugprone-sizeof-expression)
		darray_push(statements, declaration(parser));
	}

	consume(parser, Token_RightBrace);

	return ast_stmt_block(statements);
}

static Stmt *while_stmt(Parser *parser)
{
	Expr *cond = expression(parser);

	consume(parser, Token_LeftBrace);
	Stmt *body = block_stmt(parser);

	return ast_stmt_while(cond, body);
}

static Stmt *for_stmt(Parser *parser)
{
	Stmt *initializer = NULL;
	if (match(parser, Token_Semicolon))
	{
		initializer = NULL;
	}
	else if (match(parser, Token_Var))
	{
		initializer = var_decl(parser);
	}
	else
	{
		initializer = expr_stmt(parser);
	}

	Expr *condition = NULL;
	if (!check(parser, Token_Semicolon))
	{
		condition = expression(parser);
	}

	consume(parser, Token_Semicolon);

	Expr *increment = NULL;
	if (!check(parser, Token_LeftBrace))
	{
		increment = expression(parser);
	}
	consume(parser, Token_LeftBrace);

	Stmt *body = block_stmt(parser);

	if (increment != NULL)
	{
		// NOLINTNEXTLINE(bugprone-sizeof-expression)
		darray_push(body->block.statements, ast_stmt_expression(increment));
	}

	Stmt **while_stmts = NULL;

	if (initializer != NULL)
	{
		// NOLINTNEXTLINE(bugprone-sizeof-expression)
		darray_push(while_stmts, initializer);
	}

	if (condition == NULL)
	{
		condition = ast_expr_boolean_literal(true);
	}

	Stmt *while_stmt = ast_stmt_while(condition, body);

	// NOLINTNEXTLINE(bugprone-sizeof-expression)
	darray_push(while_stmts, while_stmt);

	return ast_stmt_block(while_stmts);
}

static Stmt *return_stmt(Parser *parser)
{
	Expr *return_expr = NULL;

	if (!check(parser, Token_Semicolon))
	{
		return_expr = expression(parser);
	}

	consume(parser, Token_Semicolon);

	return ast_stmt_return(return_expr);
}

static bool match(Parser *parser, Token_Type type)
{
	if (parser->curr_token.type == type)
	{
		advance(parser);
		return true;
	}

	return false;
}

static bool check(Parser *parser, Token_Type type)
{
	return parser->curr_token.type == type;
}

// FIXME: Find a better name
static Token consume(Parser *parser, Token_Type expected)
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
