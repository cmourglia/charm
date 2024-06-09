#include "value.h"

Value value_nil()
{
	return (Value){ .type = VALUE_NIL };
}

Value value_number(f64 n)
{
	Value result = { .type = VALUE_NUMBER };
	result.as.number = n;
	return result;
}

Value value_bool(bool b)
{
	Value result = { .type = VALUE_BOOL };
	result.as.boolean = b;
	return result;
}

Value value_string(String s)
{
	Value result = { .type = VALUE_STRING };
	result.as.string = s;
	return result;
}

Value value_function(struct Identifier *args, struct Stmt *body)
{
	Value result = { .type = VALUE_FUNCTION };
	result.as.function = (Function){
		.args = args,
		.body = body,
	};
	return result;
}

Value value_native_function(NativeFunction function)
{
	Value result = { .type = VALUE_NATIVE_FUNCTION };
	result.as.native_function = function;
	return result;
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
