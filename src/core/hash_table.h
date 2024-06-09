#pragma once

#include "common.h"

#include "ast/ast.h"

#include "value.h"

typedef Identifier Key;

typedef struct Entry
{
	Key key;
	Value value;
} Entry;

typedef struct HashTable
{
	int count;
	int capacity;
	struct Entry *entries;
} HashTable;

void hash_table_init(HashTable *table);
void hash_table_free(HashTable *table);

bool hash_table_set(HashTable *table, Key key, Value value);
bool hash_table_get(HashTable *table, Key key, Value *value);
bool hash_table_delete(HashTable *table, Key key);
