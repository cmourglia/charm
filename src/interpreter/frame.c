#include "frame.h"

#include "core/dyn_array.h"
#include "core/hash_table.h"

typedef struct Frame
{
	HashTable variables;
} Frame;

void frame_stack_init(FrameStack *stack)
{
	*stack = (FrameStack){};
}

void frame_stack_free(FrameStack *stack)
{
	arrfree(stack->frames);
	stack->frames = NULL;
}

void frame_stack_push_frame(FrameStack *stack)
{
	Frame frame;
	hash_table_init(&frame.variables);

	arrpush(stack->frames, frame);
}

void frame_stack_pop_frame(FrameStack *stack)
{
	if (arrempty(stack->frames))
	{
		return;
	}

	Frame top = arrpop(stack->frames);
	hash_table_free(&top.variables);
}

bool frame_stack_get_value(FrameStack *stack, String *identifier, Value *value)
{
	i32 stack_len = (i32)arrlen(stack->frames);

	*value = value_nil();

	for (i32 i = stack_len - 1; i >= 0; i--)
	{
		Frame *frame = &stack->frames[i];
		if (hash_table_get(&frame->variables, identifier, value))
		{
			return true;
		}
	}

	return false;
}

void frame_stack_declare_variable(FrameStack *stack, String *identifier,
								  Value value)
{
	Frame *frame = &arrlast(stack->frames);
	hash_table_set(&frame->variables, identifier, value);
}

bool frame_stack_set_variable(FrameStack *stack, String *identifier,
							  Value value)
{
	i32 stack_len = (i32)arrlen(stack->frames);

	for (int i = stack_len - 1; i >= 0; i--)
	{
		Value old_value = value_nil();
		Frame *frame = &stack->frames[i];

		if (hash_table_get(&frame->variables, identifier, &old_value))
		{
			if (old_value.type == VALUE_NIL || old_value.type == value.type)
			{
				hash_table_set(&frame->variables, identifier, value);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}
