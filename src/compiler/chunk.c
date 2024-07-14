#include "chunk.h"

#include "core/value.h"
#include "core/dyn_array.h"

void chunk_init(Chunk *chunk)
{
	chunk->code = NULL;
	chunk->constants = NULL;
}

void chunk_free(Chunk *chunk)
{
	arrfree(chunk->constants);
	arrfree(chunk->code);
}

void chunk_write(Chunk *chunk, u8 byte)
{
	arrpush(chunk->code, byte);
}

void chunk_write_constant(Chunk *chunk, Value value)
{
	u16 loc = chunk_add_constant(chunk, value);
	assert(loc < 256 && "TODO: Handle more than 256 constants");

	chunk_write(chunk, OP_CONSTANT);
	chunk_write(chunk, (u8)loc);
}

u16 chunk_add_constant(Chunk *chunk, struct Value value)
{
	arrpush(chunk->constants, value);
	return (u16)arrlen(chunk->constants) - 1;
}
