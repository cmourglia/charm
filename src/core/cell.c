#include "core/cell.h"

#include "core/memory.h"
#include "core/value.h"

bool cell_is_of_type(Value value, CellType type)
{
	return is_cell(value) && value.as.cell->type == type;
}

static void string_sanitize(String *string);

#define ALLOC_CELL(type, cell_type, additional_size) \
	(type *)allocate_cell(sizeof(type), cell_type, additional_size);

static Cell *allocate_cell(usize size, CellType type, usize additional_size)
{
	Cell *cell = mem_malloc(size + additional_size);
	cell->type = type;
	return cell;
}

static String *allocate_string(const char *str, i32 len)
{
	String *string = ALLOC_CELL(String, CELL_STRING, len + 1);
	string->len = len;

	memcpy(string->str, str, len);
	string->str[len] = '\0';

	return string;
}

String *string_from(const char *str, i32 len)
{
	String *string = allocate_string(str, len);

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
