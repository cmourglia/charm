#pragma once

#include "core/common.h"

struct Value;

typedef enum OpCode : u8
{
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
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

void chunk_write_constant(Chunk *chunk, struct Value value);
