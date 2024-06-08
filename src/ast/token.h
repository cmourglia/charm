#pragma once

#include "core/common.h"

typedef enum Token_Type
{
	Token_EOF,
	Token_Invalid,
	Token_Comment,

	Token_LeftParen,
	Token_RightParen,
	Token_LeftBrace,
	Token_RightBrace,
	Token_LeftBracket,
	Token_RightBracket,

	Token_Comma,
	Token_Dot,
	Token_Semicolon,

	// TODO: Add += -= *= /=
	Token_Minus,
	Token_Plus,
	Token_Slash,
	Token_Star,

	Token_BangEqual,
	Token_Equal,
	Token_EqualEqual,
	Token_Greater,
	Token_GreaterEqual,
	Token_Less,
	Token_LessEqual,

	Token_Identifier,
	Token_String,
	Token_Number,

	Token_And,
	Token_Else,
	Token_False,
	Token_For,
	Token_Function,
	Token_If,
	Token_Nil,
	Token_Not,
	Token_Or,
	Token_Return,
	Token_Struct,
	Token_True,
	Token_Var,
	Token_While,

	// Remove at some point
	Token_Super,
	Token_This,
} Token_Type;

typedef struct Token
{
	const char *lexeme_start;
	Token_Type type;
	int lexeme_len;
} Token;
