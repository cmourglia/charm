#include "value.h"

Value value_nil()
{
	return (Value){ .type = VALUE_NIL };
}

Value value_number(f64 n)
{
	return (Value){
		.number = n,
		.type = VALUE_NUMBER,
	};
}

Value value_bool(bool b)
{
	return (Value){
		.boolean = b,
		.type = VALUE_BOOL,
	};
}

Value value_string(String s)
{
	return (Value){
		.string = s,
		.type = VALUE_STRING,
	};
}

Value value_function(struct Identifier *args, struct Stmt *body)
{
	return (Value){
		.function = {
            .args = args,
            .body = body,
        },
		.type = VALUE_FUNCTION,
	};
}

Value value_native_function(NativeFunction function)
{
	return (Value) {
        .native_function = {
            .function = function,
        },
        .type = VALUE_NATIVE_FUNCTION,
    };
}

Result result_none()
{
	return (Result){ .type = RESULT_NONE };
}

Result result_return(Value value)
{
	return (Result){
		.return_result = { .value = value },
		.type = RESULT_RETURN,
	};
}
