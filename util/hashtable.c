#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

/** 
* @brief 
*/
/** 
* @brief 
* 
* @param key
* 
* @return 
*/
static unsigned int sdbmhasher(const char *key)
{
	unsigned long hash = 0;
	while (*key)
		hash = *key++ + (hash << 6) + (hash << 16) - hash;
	return hash;
}

/** 
* @brief 
* 
* @param size
* @param hasher
* @param table
*/
void hashtable_init(hashtable_t *table, unsigned int size, hashfunc hasher)
{
	int memsize = sizeof(struct hashnode_t *) * size;

	table->nodes = malloc(memsize);
	memset(table->nodes, 0, memsize);
	table->size = size;
	table->hasher = hasher ? hasher : sdbmhasher;
}

/** 
* @brief 
* 
* @param table
*/
void hashtable_cleanup(hashtable_t *table, cleanup_func clean)
{
	int i;
	struct hashnode_t *node, *prev;

	for (i = 0; i < table->size; ++i) {
		node = table->nodes[i];
		while (node) {
			prev = node;
			node = node->next;
			if (clean)
				clean(prev->val);
			free(prev->key);
			free(prev);
		}
	}
	free(table->nodes);
}

/** 
* @brief 
* 
* @param table
* @param key
* @param data
*/
void hashtable_add(hashtable_t *table, const char *key, void *data)
{
	struct hashnode_t *node;
	unsigned int hash = table->hasher(key) % table->size;

	node = table->nodes[hash];
	while (node) {
		if (node->key && strcmp(node->key, key) == 0) {
			node->val = data;
			return;
		}
		node = node->next;
	}
	node = malloc(sizeof(struct hashnode_t));
	node->key = strdup(key);
	node->val = data;
	node->next = table->nodes[hash];
	table->nodes[hash] = node;
}

/** 
* @brief Remove an entry from a hashtable.
* 
* @param table the hashtable.
* @param key the key to remove.
* 
* @return 0 on success, 1 on failure.
*/
void *hashtable_remove(hashtable_t *table, const char *key)
{
	struct hashnode_t *node, *prev = NULL;
	unsigned int hash = table->hasher(key) % table->size;
	void *ret = NULL;

	node = table->nodes[hash];
	while (node) {
		if (node->key && strcmp(node->key, key) == 0) {
			if (prev)
				prev->next = node->next;
			else
				table->nodes[hash] = node->next;

			ret = node->val;
			free(node->key);
			free(node);
			return ret;
		}
		prev = node;
		node = node->next;
	}
	return ret;
}

/** 
* @brief Retrieve a value from a hashtable.
* 
* @param table the hashtable.
* @param key the key to retrieve.
* 
* @return if present, the stored data. Otherwise NULL.
*/
void *hashtable_get(const hashtable_t *table, const char *key)
{
	struct hashnode_t *node;
	unsigned int hash = table->hasher(key) % table->size;

	node = table->nodes[hash];
	while (node) {
		if (node->key && strcmp(node->key, key) == 0)
			return node->val;
		node = node->next;
	}
	return NULL;
}
