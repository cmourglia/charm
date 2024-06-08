#include "value.h"

Value value_nil()
{
	return (Value){ .value_type = Value_Nil };
}

Value value_number(f64 n)
{
	return (Value){
		.number = n,
		.value_type = Value_Number,
	};
}

Value value_bool(bool b)
{
	return (Value){
		.boolean = b,
		.value_type = Value_Bool,
	};
}

Value value_string(String s)
{
	return (Value){
		.string = s,
		.value_type = Value_String,
	};
}

Value value_function(struct Identifier *args, struct Stmt *body)
{
	return (Value){
		.function = {
            .args = args,
            .body = body,
        },
		.value_type = Value_Function,
	};
}

Value value_native_function(NativeFunction function)
{
	return (Value) {
        .native_function = {
            .function = function,
        },
        .value_type = Value_NativeFunction,
    };
}

Result result_none()
{
	return (Result){ .result_type = Result_None };
}

Result result_return(Value value)
{
	return (Result){
		.return_result = { .value = value },
		.result_type = Result_Return,
	};
}
