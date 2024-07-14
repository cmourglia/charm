#include "vm.h"

#include "core/common.h"
#include "core/dyn_array.h"
#include "core/hash_table.h"
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
	hash_table_init(&vm.globals);
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
#define READ_SHORT() (vm.ip += 2, (u16)((vm.ip[-2] << 8) | vm.ip[-1]))
#define READ_CONSTANT() (vm.chunk->constants[READ_BYTE()])
#define READ_STRING() as_string(READ_CONSTANT())

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
			if (is_cell(*slot))
			{
				printf(" (%p)", as_cell(*slot));
			}
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

			case OP_NIL:
			{
				push(value_nil());
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

			case OP_POP:
			{
				pop();
			}
			break;

			case OP_DEFINE_GLOBAL:
			{
				String *name = READ_STRING();
				hash_table_set(&vm.globals, name, peek(0));
				pop();
			}
			break;

			case OP_GET_GLOBAL:
			{
				String *name = READ_STRING();
				Value value;
				if (!hash_table_get(&vm.globals, name, &value))
				{
					printf("Undefined variable %s\n", name->str);
					return INTERPRET_RUNTIME_ERROR;
				}
				push(value);
			}
			break;

			case OP_SET_GLOBAL:
			{
				String *name = READ_STRING();
				Value value = peek(0);

				Value old_value;
				if (!hash_table_get(&vm.globals, name, &old_value))
				{
					printf("Undefined variable %s\n", name->str);
					return INTERPRET_RUNTIME_ERROR;
				}

				if (!is_nil(old_value) && !values_share_type(old_value, value))
				{
					// TODO: This should be handled by typechecking
					printf("Trying to assign to incompatible types\n");
					return INTERPRET_RUNTIME_ERROR;
				}

				hash_table_set(&vm.globals, name, value);
			}
			break;

			case OP_GET_LOCAL:
			{
				u8 slot = READ_BYTE();
				push(vm.stack[slot]);
			}
			break;

			case OP_SET_LOCAL:
			{
				u8 slot = READ_BYTE();
				vm.stack[slot] = peek(0);
			}
			break;

			case OP_JUMP:
			{
				u16 offset = READ_SHORT();
				vm.ip += offset;
			}
			break;

			case OP_LOOP:
			{
				u16 offset = READ_SHORT();
				vm.ip -= offset;
			}
			break;

			case OP_JUMP_IF_FALSE:
			{
				u16 offset = READ_SHORT();
				Value value = peek(0);

				if (!is_bool(value))
				{
					printf("Cannot evaluate non bool values\n");
					return INTERPRET_RUNTIME_ERROR;
				}

				if (!as_bool(value))
				{
					vm.ip += offset;
				}
			}
			break;

			case OP_RETURN:
			{
				//Value result = pop();
				//print_value(&result);
				//printf("\n");
				return INTERPRET_OK;
			}
		}
	}

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_SHORT
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
