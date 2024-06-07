#include "chunk.h"

#include "core/value.h"
#include "core/dyn_array.h"

static i32 add_constant(Chunk *chunk, struct Value value);

void chunk_init(Chunk *chunk)
{
	chunk->code = NULL;
	chunk->constants = NULL;
}

void chunk_free(Chunk *chunk)
{
	darray_free(chunk->constants);
	darray_free(chunk->code);
}

void chunk_write(Chunk *chunk, u8 byte)
{
	darray_push(chunk->code, byte);
}

void chunk_write_constant(Chunk *chunk, Value value)
{
	i32 loc = add_constant(chunk, value);
	assert(loc < 256 && "TODO: Handle more than 256 constants");

	chunk_write(chunk, Op_Constant);
	chunk_write(chunk, loc);
}

static i32 add_constant(Chunk *chunk, struct Value value)
{
	darray_push(chunk->constants, value);
	return darray_len(chunk->constants) - 1;
}
