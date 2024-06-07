#pragma once

typedef struct String
{
	char *str;
	int len;
} String;

String string_init(const char *src, int len);
void string_free(String *string);

void string_sanitize(String *string);
