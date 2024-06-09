#include "vm.h"

#include "core/common.h"
#include "core/dyn_array.h"
#include "core/value.h"

#include "compiler/chunk.h"

#include "debug/debug.h"

#define DEBUG_TRACE_EXECUTION

static Vm vm;

static InterpretResult run();

static void push(Value value);
static Value pop();
static Value peek(usize offset);

void vm_init()
{
	vm.chunk = NULL;
	vm.stack_top = vm.stack;
}

void vm_free()
{
	vm.chunk = NULL;
}

InterpretResult vm_interpret(Chunk *chunk)
{
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;

	return run();
}

static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants[READ_BYTE()])

#define BINARY_OP(op, type)                             \
	do                                                  \
	{                                                   \
		if (!is_number(peek(0)) || !is_number(peek(1))) \
		{                                               \
			/* TODO: Typechecking */                    \
			UNREACHABLE();                              \
		}                                               \
		f64 b = as_number(pop());                       \
		f64 a = as_number(pop());                       \
		push(type(a op b));                             \
	} while (false)

	for (;;)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (Value *slot = vm.stack; slot < vm.stack_top; slot++)
		{
			printf("[ ");
			print_value(slot);
			printf(" ]");
		}

		printf("\n");

		debug_disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

		u8 instruction;

		switch (instruction = READ_BYTE())
		{
			case OP_CONSTANT:
			{
				push(READ_CONSTANT());
			}
			break;

			case OP_TRUE:
			{
				push(value_bool(true));
			}
			break;

			case OP_FALSE:
			{
				push(value_bool(false));
			}
			break;

			case OP_NEGATE:
			{
				if (!is_number(peek(0)))
				{
					// TODO: Typecheck
					UNREACHABLE();
				}
				push(value_number(-as_number(pop())));
			}
			break;

			case OP_ADD:
			{
				BINARY_OP(+, value_number);
			}
			break;

			case OP_SUBTRACT:
			{
				BINARY_OP(-, value_number);
			}
			break;

			case OP_MULTIPLY:
			{
				BINARY_OP(*, value_number);
			}
			break;

			case OP_DIVIDE:
			{
				BINARY_OP(/, value_number);
			}
			break;

			case OP_NOT:
			{
				if (!is_bool(peek(0)))
				{
					// TODO: Typecheck
					UNREACHABLE();
				}

				push(value_bool(!as_bool(pop())));
			}
			break;

			case OP_AND:
			{
				UNREACHABLE();
			}
			break;

			case OP_OR:
			{
				UNREACHABLE();
			}
			break;

			case OP_EQUAL:
			{
				Value b = pop();
				Value a = pop();
				push(value_bool(values_equal(a, b)));
			}
			break;

			case OP_GREATER:
			{
				BINARY_OP(>, value_bool);
			}
			break;

			case OP_LESS:
			{
				BINARY_OP(<, value_bool);
			}
			break;

			case OP_RETURN:
			{
				Value result = pop();
				print_value(&result);
				printf("\n");
				return INTERPRET_OK;
			}
		}
	}

	UNREACHABLE();

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

static void push(Value value)
{
	*vm.stack_top++ = value;
}

static Value pop()
{
	vm.stack_top--;
	return *vm.stack_top;
}

static Value peek(usize offset)
{
	return vm.stack_top[-1 - offset];
}
