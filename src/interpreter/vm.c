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

#define BINARY_OP(op)                             \
	do                                            \
	{                                             \
		Value b = pop();                          \
		Value a = pop();                          \
		push(value_number(a.number op b.number)); \
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
				Value constant = READ_CONSTANT();
				push(constant);
			}
			break;

			case OP_NEGATE:
			{
				push(value_number(-pop().number));
			}
			break;

			case OP_ADD:
			{
				BINARY_OP(+);
			}
			break;

			case OP_SUBTRACT:
			{
				BINARY_OP(-);
			}
			break;

			case OP_MULTIPLY:
			{
				BINARY_OP(*);
			}
			break;

			case OP_DIVIDE:
			{
				BINARY_OP(/);
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
