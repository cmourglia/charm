#pragma once

#include "common.h"
#include "string.h"

typedef enum Value_Type
{
	VALUE_NIL = 0,
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_STRING,
	VALUE_USER_TYPE,
	VALUE_FUNCTION,
	VALUE_NATIVE_FUNCTION,
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

#define VALUE_IS_NIL(v) ((v).value_type == VALUE_NIL)

// TODO: Statement result
typedef enum Result_Type
{
	RESULT_NONE,
	RESULT_RETURN,
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
