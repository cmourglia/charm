#include "frame.h"

#include "core/dyn_array.h"
#include "core/hash_table.h"

typedef struct Frame
{
	Hash_Table variables;
} Frame;

void frame_stack_init(Frame_Stack *stack)
{
	*stack = (Frame_Stack){};
}

void frame_stack_free(Frame_Stack *stack)
{
	darray_free(stack->frames);
	stack->frames = NULL;
}

void frame_stack_push_frame(Frame_Stack *stack)
{
	Frame frame;
	hash_table_init(&frame.variables);

	darray_push(stack->frames, frame);
}

void frame_stack_pop_frame(Frame_Stack *stack)
{
	if (darray_empty(stack->frames))
	{
		return;
	}

	Frame top = darray_pop(stack->frames);
	hash_table_free(&top.variables);
}

bool frame_stack_get_value(Frame_Stack *stack, Identifier identifier,
						   Value *value)
{
	int stack_len = darray_len(stack->frames);

	*value = value_nil();

	for (int i = stack_len - 1; i >= 0; i--)
	{
		Frame *frame = &stack->frames[i];
		if (hash_table_get(&frame->variables, identifier, value))
		{
			return true;
		}
	}

	return false;
}

void frame_stack_declare_variable(Frame_Stack *stack, Identifier identifier,
								  Value value)
{
	Frame *frame = &darray_last(stack->frames);
	hash_table_set(&frame->variables, identifier, value);
}

bool frame_stack_set_variable(Frame_Stack *stack, Identifier identifier,
							  Value value)
{
	int stack_len = darray_len(stack->frames);

	for (int i = stack_len - 1; i >= 0; i--)
	{
		Value old_value = value_nil();
		Frame *frame = &stack->frames[i];

		if (hash_table_get(&frame->variables, identifier, &old_value))
		{
			if (old_value.value_type == Value_Nil ||
				old_value.value_type == value.value_type)
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
