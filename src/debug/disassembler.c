#include "debug.h"

#include <stdio.h>

#include "core/common.h"
#include "core/value.h"
#include "core/dyn_array.h"

#include "compiler/chunk.h"

static i32 simple_instruction(const char *name, i32 offset);
static i32 constant_instruction(const char *name, Chunk *chunk, i32 offset);

void debug_disassemble_chunk(Chunk *chunk, const char *name)
{
	printf("== %s ==\n", name);

	for (i32 offset = 0; offset < darray_len(chunk->code);)
	{
		offset = debug_disassemble_instruction(chunk, offset);
	}
}

i32 debug_disassemble_instruction(Chunk *chunk, i32 offset)
{
	printf("%04d ", offset);

	u8 instruction = chunk->code[offset];
	switch (instruction)
	{
		case OP_CONSTANT:
			return constant_instruction("OP_CONSTANT", chunk, offset);

		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);

		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
	}
}

static i32 simple_instruction(const char *name, i32 offset)
{
	printf("%s\n", name);
	return offset + 1;
}

static i32 constant_instruction(const char *name, Chunk *chunk, i32 offset)
{
	u8 constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	print_value(&chunk->constants[constant]);
	printf("'\n");
	return offset + 2;
}
