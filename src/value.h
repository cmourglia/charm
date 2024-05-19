#pragma once

#include "common.h"

enum ValueType {
	Value_Nil = 0,
	Value_Number,
	Value_Bool,
	Value_String,
	Value_UserType,
};
typedef enum ValueType ValueType;

struct Value {
	union {
		f64 number;
		bool boolean;
		// TODO: others
	};

	ValueType value_type;
};
typedef struct Value Value;

Value value_nil();
Value value_number(f64 n);
Value value_bool(bool b);

#define VALUE_IS_NIL(v) ((v).value_type == Value_Nil)
