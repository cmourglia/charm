#include "debug.h"

#include <stdio.h>

#include "core/common.h"
#include "core/value.h"
#include "core/dyn_array.h"

#include "compiler/chunk.h"

static i32 disassemble_instruction(Chunk *chunk, i32 offset);
static i32 simple_instruction(const char *name, i32 offset);
static i32 constant_instruction(const char *name, Chunk *chunk, i32 offset);

static void print_value(Value value);

void debug_disassemble_chunk(Chunk *chunk, const char *name)
{
	printf("== %s ==\n", name);

	for (i32 offset = 0; offset < darray_len(chunk->code);)
	{
		offset = disassemble_instruction(chunk, offset);
	}
}

static i32 disassemble_instruction(Chunk *chunk, i32 offset)
{
	printf("%04d ", offset);

	u8 instruction = chunk->code[offset];
	switch (instruction)
	{
		case Op_Return:
			return simple_instruction("OP_RETURN", offset);

		case Op_Constant:
			return constant_instruction("OP_CONSTANT", chunk, offset);

		default:
			printf("Unknow opcode %d\n", instruction);
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
	print_value(chunk->constants[constant]);
	printf("'\n");
	return offset + 2;
}

static void print_value(Value value)
{
	switch (value.value_type)
	{
		case Value_Nil:
		{
			printf("<NIL>");
		}
		break;

		case Value_Bool:
		{
			printf("%s", (value.boolean ? "true" : "false"));
		}
		break;

		case Value_Number:
		{
			printf("%f", value.number);
		}
		break;

		default:
			UNREACHABLE();
	}
}
