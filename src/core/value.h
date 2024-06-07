#pragma once

#include "common.h"
#include "string.h"

enum ValueType
{
	Value_Nil = 0,
	Value_Number,
	Value_Bool,
	Value_String,
	Value_UserType,
	Value_Function,
	Value_NativeFunction,
};
typedef enum ValueType ValueType;

struct Frame_Stack;

typedef struct Result (*NativeFunction)(struct Frame_Stack *);

struct Value
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
			struct Identifier *args;
			NativeFunction function;
		} native_function;
	};

	ValueType value_type;
};
typedef struct Value Value;

Value value_nil();
Value value_number(f64 n);
Value value_bool(bool b);
Value value_string(String s);
Value value_function(struct Identifier *args, struct Stmt *body);
Value value_native_function(struct Identifier *args, NativeFunction function);

#define VALUE_IS_NIL(v) ((v).value_type == Value_Nil)

// TODO: Statement result
enum Result_Type
{
	Result_None,
	Result_Return,
	// TODO: Continue,
	// TODO: Break,
	// TODO: Error ?
};
typedef enum Result_Type Result_Type;

struct Result
{
	union
	{
		struct
		{
			Value value;
		} return_result;
	};

	Result_Type result_type;
};
typedef struct Result Result;

Result result_none();
Result result_return(Value value);
