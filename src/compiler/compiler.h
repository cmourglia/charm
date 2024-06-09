#pragma once

struct Program;
struct Chunk;

typedef enum CompileResult
{
	COMPILE_OK,
	// TODO:
} CompileResult;

CompileResult compile_program(struct Chunk *chunk, struct Program program);
