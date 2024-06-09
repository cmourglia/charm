#pragma once

#include "core/common.h"
#include "core/value.h"

struct Chunk;

typedef enum InterpretResult
{
	INTERPRET_OK,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

// TODO: Dynamic stack at some point ?
#define STACK_MAX 256

typedef struct Vm
{
	struct Chunk *chunk;
	u8 *ip;

	Value stack[STACK_MAX];
	Value *stack_top;
} Vm;

void vm_init();
void vm_free();

InterpretResult vm_interpret(struct Chunk *chunk);
