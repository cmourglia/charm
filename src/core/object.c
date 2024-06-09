#include "core/object.h"

#include "core/memory.h"
#include "core/value.h"

bool object_is_of_type(Value value, ObjType type)
{
	return is_object(value) && value.as.object->type == type;
}

static void string_sanitize(String *string);

#define ALLOC_OBJ(type, object_type) \
	(type *)allocate_object(sizeof(type), object_type);

static Object *allocate_object(usize size, ObjType type)
{
	Object *object = mem_malloc(size);
	object->type = type;
	return object;
}

static String *allocate_string(char *str, i32 len)
{
	String *string = ALLOC_OBJ(String, OBJ_STRING);
	string->str = str;
	string->len = len;
	return string;
}

String *string_from(const char *str, i32 len)
{
	char *heap_str = mem_malloc(len + 1);
	memcpy(heap_str, str, len);

	String *string = allocate_string(heap_str, len);

	string_sanitize(string);
	return string;
}

static void string_sanitize(String *string)
{
	int read = 0;
	int write = 0;

	for (; read < string->len;)
	{
		char c = string->str[read++];
		if (c == '\\')
		{
			switch (string->str[read++])
			{
				case 'n':
					c = '\n';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				case '\\':
					c = '\\';
					break;
			}
		}

		string->str[write++] = c;
	}

	string->len = write;
	string->str[write] = '\0';
}
