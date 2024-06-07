#pragma once

#include "core/common.h"

struct Value;

enum Op_Code : u8
{
	Op_Return,
	Op_Constant,
};
typedef enum Op_Code Op_Code;

struct Chunk
{
	u8 *code;
	struct Value *constants;
};
typedef struct Chunk Chunk;

void chunk_init(Chunk *chunk);
void chunk_free(Chunk *chunk);

void chunk_write(Chunk *chunk, u8 byte);

void chunk_write_constant(Chunk *chunk, struct Value value);
