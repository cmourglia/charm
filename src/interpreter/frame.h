#pragma once

#include "core/value.h"
#include "core/cell.h"

typedef struct FrameStack
{
	struct Frame *frames;
} FrameStack;

void frame_stack_init(FrameStack *stack);
void frame_stack_free(FrameStack *stack);

void frame_stack_push_frame(FrameStack *stack);
void frame_stack_pop_frame(FrameStack *stack);

bool frame_stack_get_value(FrameStack *stack, String *identifier, Value *value);

void frame_stack_declare_variable(FrameStack *stack, String *identifier,
								  Value value);
bool frame_stack_set_variable(FrameStack *stack, String *identifier,
							  Value value);
