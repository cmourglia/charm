#include "core/cell.h"

#include "core/common.h"
#include "core/memory.h"
#include "core/value.h"
#include "core/hash_table.h"

bool cell_is_of_type(Value value, CellType type)
{
	return is_cell(value) && value.as.cell->type == type;
}

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

static const char *string_sanitize(const char *str, i32 *str_len);
static bool needs_sanitization(const char *str, i32 len);

String *string_from_str(HashTable *strings, const char *str, i32 len)
{
	const char *str_cleaned = str;
	i32 len_cleaned = len;
	bool sanitized = false;

	if (needs_sanitization(str, len))
	{
		str_cleaned = string_sanitize(str, &len_cleaned);
		sanitized = true;
	}

	String *string = hash_table_find_key(strings, str_cleaned, len_cleaned);

	if (string == NULL)
	{
		string = allocate_string(str_cleaned, len_cleaned);
		hash_table_set(strings, string, value_nil());
	}

	if (sanitized)
	{
		free((char *)str_cleaned);
	}

	return string;
}

String *string_from_cstr(HashTable *strings, const char *str)
{
	return string_from_str(strings, str, (i32)strlen(str));
}

static bool needs_sanitization(const char *str, i32 len)
{
	for (i32 i = 0; i < len - 1; i++)
	{
		if (str[i] == '\\')
		{
			switch (str[i + 1])
			{
				case 'n':
				case 'r':
				case 't':
				case '\\':
					return true;
			}
		}
	}

	return false;
}

const char *string_sanitize(const char *str, i32 *str_len)
{
	i32 read = 0;
	i32 write = 0;

	char *string = malloc(*str_len);
	memcpy(string, str, *str_len);

	for (; read < *str_len;)
	{
		char c = string[read++];
		if (c == '\\')
		{
			switch (string[read++])
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
				default:
					UNREACHABLE();
			}
		}

		string[write++] = c;
	}

	*str_len = write;
	return string;
}
