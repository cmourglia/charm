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
