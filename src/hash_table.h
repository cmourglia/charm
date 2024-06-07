#pragma once

#include "common.h"

#include "ast.h"
#include "value.h"

typedef Identifier Key;

typedef struct Entry
{
	Key key;
	Value value;
} Entry;

typedef struct Hash_Table
{
	int count;
	int capacity;
	struct Entry *entries;
} Hash_Table;

void hash_table_init(Hash_Table *table);
void hash_table_free(Hash_Table *table);

bool hash_table_set(Hash_Table *table, Key key, Value value);
bool hash_table_get(Hash_Table *table, Key key, Value *value);
bool hash_table_delete(Hash_Table *table, Key key);
