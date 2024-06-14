#pragma once

#include "core/common.h"

struct Value;

typedef enum OpCode : u8
{
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NOT,
	OP_AND,
	OP_OR,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_POP,
	// TODO: Add a OP_POPN for batch popping
	OP_DEFINE_GLOBAL,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_SET_LOCAL,
	OP_GET_LOCAL,
	OP_JUMP,
	OP_JUMP_IF_FALSE,
	OP_LOOP,
	OP_RETURN,
} OpCode;

typedef struct Chunk
{
	u8 *code;
	struct Value *constants;
} Chunk;

void chunk_init(Chunk *chunk);
void chunk_free(Chunk *chunk);

void chunk_write(Chunk *chunk, u8 byte);

u16 chunk_add_constant(Chunk *chunk, struct Value value);
