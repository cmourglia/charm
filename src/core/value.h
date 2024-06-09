#pragma once

#include "common.h"
#include "string.h"

typedef enum ValueType
{
	VALUE_NIL = 0,
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_STRING,
	VALUE_USER_TYPE,
	VALUE_FUNCTION,
	VALUE_NATIVE_FUNCTION,
} ValueType;

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

	ValueType type;
} Value;

Value value_nil();
Value value_number(f64 number);
Value value_bool(bool boolean);
Value value_string(String string);
Value value_function(struct Identifier *args, struct Stmt *body);
Value value_native_function(NativeFunction function);

#define VALUE_IS_NIL(v) ((v).type == VALUE_NIL)

// TODO: Statement result
typedef enum ResultType
{
	RESULT_NONE,
	RESULT_RETURN,
	// TODO: Continue,
	// TODO: Break,
	// TODO: Error ?
} ResultType;

typedef struct Result
{
	union
	{
		struct
		{
			Value value;
		} return_result;
	};

	ResultType type;
} Result;

Result result_none();
Result result_return(Value value);
