#include "string.h"

#include "memory.h"
#include "dyn_array.h"

#include <stdio.h>

String string_init(const char *src, int len)
{
	String string = { .str = mem_malloc(len), .len = len };

	mem_copy(string.str, src, len);

	return string;
}

void string_free(String *string)
{
	mem_free(string->str);
	string->str = NULL;
	string->len = 0;
}

void string_sanitize(String *string)
{
	char *out = NULL;

	for (int i = 0; i < string->len; i++)
	{
		char c = string->str[i];
		if (c == '\\')
		{
			switch (string->str[i + 1])
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

			i += 1;
		}

		darray_push(out, c);
	}

	assert(string->len >= darray_len(out));
	mem_copy(string->str, out, darray_len(out));
	string->len = darray_len(out);

	darray_free(out);
}
