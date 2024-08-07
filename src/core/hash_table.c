#include "hash_table.h"

#include <string.h>

#include "value.h"
#include "memory.h"

#define TABLE_MAX_LOAD 0.75
static u32 hash_string(const char *str, i32 len)
{
	// 32-bits FNV-1a hash
	// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
	u32 hash = 2166136261u;

	for (int i = 0; i < len; i++)
	{
		hash ^= (u8)str[i];
		hash *= 16777619;
	}

	return hash;
}

void hash_table_init(HashTable *table)
{
	mem_zero(table, HashTable, 1);
}

void hash_table_free(HashTable *table)
{
	mem_free_array(Entry, table->entries);
	hash_table_init(table);
}

static void adjust_capacity(HashTable *table, int new_capacity);
static Entry *find_entry(Entry *entries, int capacity, Key key);

static bool is_null_entry(Entry *entry);
static bool keys_equal(Key a, Key b);

bool hash_table_set(HashTable *table, Key key, Value value)
{
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
	{
		int capacity = mem_grow_capacity(table->capacity, 0);
		adjust_capacity(table, capacity);
	}

	Entry *entry = find_entry(table->entries, table->capacity, key);

	bool is_new_key = entry->key == NULL;

	if (is_new_key && is_nil(entry->value))
	{
		table->count += 1;
	}

	entry->key = key;
	entry->value = value;

	return is_new_key;
}

bool hash_table_get(HashTable *table, Key key, Value *value)
{
	if (table->count == 0)
	{
		return false;
	}

	Entry *entry = find_entry(table->entries, table->capacity, key);
	if (is_null_entry(entry))
	{
		return false;
	}

	*value = entry->value;

	return true;
}

bool hash_table_delete(HashTable *table, Key key)
{
	if (table->count == 0)
	{
		return false;
	}

	Entry *entry = find_entry(table->entries, table->capacity, key);
	if (is_null_entry(entry))
	{
		return false;
	}

	// Tombstone
	entry->key = (Key){ 0 };
	entry->value = value_bool(true);

	return true;
}

String *hash_table_find_key(HashTable *table, const char *str, i32 len)
{
	if (table->count == 0)
	{
		return NULL;
	}

	u32 index = hash_string(str, len) % table->capacity;

	for (;;)
	{
		Entry *entry = &table->entries[index];

		if (entry->key == NULL)
		{
			if (is_nil(entry->value))
			{
				return NULL;
			}
		}
		else if (entry->key->len == len &&
				 memcmp(entry->key->str, str, len) == 0)
		{
			return entry->key;
		}

		index = (index + 1) % table->capacity;
	}
}

static void adjust_capacity(HashTable *table, int new_capacity)
{
	Entry *entries = mem_allocate(Entry, new_capacity);

	mem_zero(entries, Entry, new_capacity);

	table->count = 0;

	for (int i = 0; i < table->capacity; i++)
	{
		if (is_null_entry(table->entries + i))
		{
			continue;
		}

		Entry *entry = find_entry(entries, new_capacity, table->entries[i].key);
		entry->key = table->entries[i].key;
		entry->value = table->entries[i].value;

		table->count += 1;
	}

	mem_free_array(Entry, table->entries);

	table->entries = entries;
	table->capacity = new_capacity;
}

static Entry *find_entry(Entry *entries, int capacity, Key key)
{
	u32 index = hash_string(key->str, key->len) % capacity;

	Entry *tombstone = NULL;

	for (;;)
	{
		Entry *entry = &entries[index];

		if (is_null_entry(entry))
		{
			// If we find a trully empty entry, we either return
			// the first tombstone we found, or this one if none
			// have been encountered
			if (is_nil(entry->value))
			{
				return tombstone != NULL ? tombstone : entry;
			}
			else
			{
				if (tombstone == NULL)
				{
					tombstone = entry;
				}
			}
		}
		else if (keys_equal(entry->key, key))
		{
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static bool is_null_entry(Entry *entry)
{
	return entry->key == NULL;
}

static bool keys_equal(Key a, Key b)
{
	return a == b;
}
