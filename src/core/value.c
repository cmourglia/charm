#include "value.h"

#include "core/common.h"
#include "core/cell.h"

Value value_nil()
{
	return (Value){ .type = VALUE_NIL };
}

Value value_number(f64 n)
{
	return (Value){ .type = VALUE_NUMBER, { .number = n } };
}

Value value_bool(bool b)
{
	return (Value){ .type = VALUE_BOOL, { .boolean = b } };
}

Value value_cell(Cell *cell)
{
	return (Value){ .type = VALUE_CELL, { .cell = cell } };
}

Value value_function(struct String **args, struct Stmt *body)
{
	Function function = { .args = args, .body = body };
	return (Value){ .type = VALUE_FUNCTION, { .function = function } };
}

Value value_native_function(NativeFunction function)
{
	return (Value){
		.type = VALUE_NATIVE_FUNCTION,
		{ .native_function = function },
	};
}

bool values_equal(Value a, Value b)
{
	if (!values_share_type(a, b))
	{
		return false;
	}

	switch (a.type)
	{
		case VALUE_NIL:
			return true;

		case VALUE_BOOL:
			return as_bool(a) == as_bool(b);

		case VALUE_NUMBER:
			return as_number(a) == as_number(b);

		case VALUE_CELL:
			return as_cell(a) == as_cell(b);

		default:
			UNREACHABLE();
	}
}

Result result_none()
{
	return (Result){ .type = RESULT_NONE };
}

Result result_return(Value value)
{
	Result result = { .type = RESULT_RETURN };
	result.as.return_result = value;
	return result;
}
