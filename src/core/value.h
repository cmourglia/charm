#pragma once

#include "common.h"

typedef enum ValueType
{
	VALUE_NIL = 0,
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_CELL,
	VALUE_USER_TYPE,
	VALUE_FUNCTION,
	VALUE_NATIVE_FUNCTION,
} ValueType;

struct Value;
typedef struct Result (*NativeFunction)(struct Value *);

// GC-ed objects
struct Cell;

typedef struct Function
{
	struct Identifier *args;
	struct Stmt *body;
} Function;

typedef struct Value
{
	ValueType type;

	union
	{
		f64 number;
		bool boolean;
		struct Cell *cell;
		NativeFunction native_function;
		Function function;
		// TODO: others
	} as;
} Value;

Value value_nil();
Value value_number(f64 number);
Value value_bool(bool boolean);
Value value_cell(struct Cell *cell);
Value value_function(struct Identifier *args, struct Stmt *body);
Value value_native_function(NativeFunction function);

#define is_nil(v) ((v).type == VALUE_NIL)
#define is_bool(v) ((v).type == VALUE_BOOL)
#define is_number(v) ((v).type == VALUE_NUMBER)
#define is_cell(v) ((v).type == VALUE_CELL)
#define is_function(v) ((v).type == VALUE_FUNCTION)
#define is_native_function(v) ((v).type == VALUE_NATIVE_FUNCTION)

#define as_bool(v) ((v).as.boolean)
#define as_number(v) ((v).as.number)
#define as_cell(v) ((v).as.cell)
#define as_function(v) ((v).as.function)
#define as_native_function(v) ((v).as.native_function)

#define values_share_type(a, b) ((a).type == (b).type)
bool values_equal(Value a, Value b);

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
		Value return_result;
	} as;

	ResultType type;
} Result;

Result result_none();
Result result_return(Value value);
