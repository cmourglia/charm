#include "debug.h"

#include <stdio.h>

#include "core/common.h"
#include "core/value.h"
#include "core/dyn_array.h"

#include "compiler/chunk.h"

static i32 simple_instruction(const char *name, i32 offset);
static i32 constant_instruction(const char *name, Chunk *chunk, i32 offset);
static i32 byte_instruction(const char *name, Chunk *chunk, i32 offset);
static i32 jump_instruction(const char *name, i32 sign, Chunk *chunk,
							i32 offset);

void debug_disassemble_chunk(Chunk *chunk, const char *name)
{
	printf("== %s ==\n", name);

	for (i32 offset = 0; offset < arrlen(chunk->code);)
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

		case OP_TRUE:
			return simple_instruction("OP_TRUE", offset);

		case OP_FALSE:
			return simple_instruction("OP_FALSE", offset);

		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", offset);

		case OP_ADD:
			return simple_instruction("OP_ADD", offset);

		case OP_SUBTRACT:
			return simple_instruction("OP_SUBTRACT", offset);

		case OP_MULTIPLY:
			return simple_instruction("OP_MULTIPLY", offset);

		case OP_DIVIDE:
			return simple_instruction("OP_DIVIDE", offset);

		case OP_NOT:
			return simple_instruction("OP_NOT", offset);

		case OP_AND:
			return simple_instruction("OP_AND", offset);

		case OP_OR:
			return simple_instruction("OP_OR", offset);

		case OP_EQUAL:
			return simple_instruction("OP_EQUAL", offset);

		case OP_GREATER:
			return simple_instruction("OP_GREATER", offset);

		case OP_LESS:
			return simple_instruction("OP_LESS", offset);

		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);

		case OP_POP:
			return simple_instruction("OP_POP", offset);

		case OP_DEFINE_GLOBAL:
			return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);

		case OP_GET_GLOBAL:
			return constant_instruction("OP_GET_GLOBAL", chunk, offset);

		case OP_SET_GLOBAL:
			return constant_instruction("OP_SET_GLOBAL", chunk, offset);

		case OP_GET_LOCAL:
			return byte_instruction("OP_GET_LOCAL", chunk, offset);

		case OP_SET_LOCAL:
			return byte_instruction("OP_SET_LOCAL", chunk, offset);

		case OP_JUMP:
			return jump_instruction("OP_JUMP", 1, chunk, offset);

		case OP_JUMP_IF_FALSE:
			return jump_instruction("OP_JUMP_IF_FALSE", 1, chunk, offset);

		case OP_LOOP:
			return jump_instruction("OP_LOOP", -1, chunk, offset);

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

static i32 byte_instruction(const char *name, Chunk *chunk, i32 offset)
{
	u8 slot = chunk->code[offset + 1];
	printf("%-16s %4d\n", name, slot);
	return offset + 2;
}

static i32 jump_instruction(const char *name, i32 sign, Chunk *chunk,
							i32 offset)
{
	u16 jump = (u16)(chunk->code[offset + 1] << 8);
	jump |= chunk->code[offset + 2];
	printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
	return offset + 3;
}
