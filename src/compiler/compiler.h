#pragma once

struct Program;
struct Chunk;

typedef enum CompileResult
{
	COMPILE_OK,
	// TODO:
} CompileResult;

typedef struct Compiler
{
	struct Chunk *chunk;
} Compiler;

void compiler_init(Compiler *compiler, struct Chunk *chunk);
void compiler_free(Compiler *compiler);

CompileResult compile_program(Compiler *compiler, struct Program program);
