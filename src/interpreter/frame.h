#pragma once

#include "core/value.h"

#include "ast/ast.h"

struct Frame_Stack
{
	struct Frame *frames;
};

typedef struct Frame_Stack Frame_Stack;

void frame_stack_init(Frame_Stack *stack);
void frame_stack_free(Frame_Stack *stack);

void frame_stack_push_frame(Frame_Stack *stack);
void frame_stack_pop_frame(Frame_Stack *stack);

bool frame_stack_get_value(Frame_Stack *stack, Identifier identifier,
						   Value *value);

void frame_stack_declare_variable(Frame_Stack *stack, Identifier identifier,
								  Value value);
bool frame_stack_set_variable(Frame_Stack *stack, Identifier identifier,
							  Value value);
