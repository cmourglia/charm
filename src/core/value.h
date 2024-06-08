#pragma once

#include "common.h"
#include "string.h"

typedef enum Value_Type
{
	Value_Nil = 0,
	Value_Number,
	Value_Bool,
	Value_String,
	Value_UserType,
	Value_Function,
	Value_NativeFunction,
} Value_Type;

struct Value;
typedef struct Result (*NativeFunction)(struct Value *);

typedef struct Value
{
	union
	{
		f64 number;
		bool boolean;
		String string;
		// TODO: others
		struct
		{
			struct Identifier *args;
			struct Stmt *body;
		} function;
		struct
		{
			NativeFunction function;
		} native_function;
	};

	Value_Type value_type;
} Value;

Value value_nil();
Value value_number(f64 number);
Value value_bool(bool boolean);
Value value_string(String string);
Value value_function(struct Identifier *args, struct Stmt *body);
Value value_native_function(NativeFunction function);

#define VALUE_IS_NIL(v) ((v).value_type == Value_Nil)

// TODO: Statement result
typedef enum Result_Type
{
	Result_None,
	Result_Return,
	// TODO: Continue,
	// TODO: Break,
	// TODO: Error ?
} Result_Type;

typedef struct Result
{
	union
	{
		struct
		{
			Value value;
		} return_result;
	};

	Result_Type result_type;
} Result;

Result result_none();
Result result_return(Value value);
