#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

/** 
* @brief 
*/
struct keypair {
	char *key;
	void *data;
	struct keypair *next;
};

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
* @param tbl
*/
void hashtable_init(unsigned int size, hashfunc hasher, hashtable *tbl)
{
	tbl->nodes = calloc(size, sizeof(struct keypair *));
	memset(tbl->nodes, 0, sizeof(struct keypair *) * size);
	tbl->size = size;
	tbl->hasher = hasher ? hasher : sdbmhasher;
}

/** 
* @brief 
* 
* @param tbl
*/
void hashtable_free(hashtable *tbl, int freedata)
{
	int i;
	struct keypair *node, *prev;

	for (i = 0; i < tbl->size; ++i) {
		node = tbl->nodes[i];
		while (node) {
			free(node->key);
			if (freedata)
				free(node->data);
			prev = node;
			node = node->next;
			free(prev);
		}
	}
	free(tbl->nodes);
}

/** 
* @brief 
* 
* @param tbl
* @param key
* @param data
*/
void hashtable_insert(hashtable *tbl, const char *key, void *data)
{
	struct keypair *node;
	unsigned int hash = tbl->hasher(key) % tbl->size;

	node = tbl->nodes[hash];
	while (node) {
		if (strcmp(node->key, key) == 0) {
			node->data = data;
			return;
		}
		node = node->next;
	}
	node = malloc(sizeof(struct keypair));
	node->key = strdup(key);
	node->data = data;
	node->next = tbl->nodes[hash];
	tbl->nodes[hash] = node;
}

/** 
* @brief Remove an entry from a hashtable.
* 
* @param tbl the hashtable.
* @param key the key to remove.
* 
* @return 0 on success, 1 on failure.
*/
int hashtable_remove(hashtable *tbl, const char *key)
{
	struct keypair *node, *prev = NULL;
	unsigned int hash = tbl->hasher(key) % tbl->size;

	node = tbl->nodes[hash];
	while (node) {
		if (strcmp(node->key, key) == 0) {
			if (prev)
				prev->next = node->next;
			else
				tbl->nodes[hash] = node->next;

			free(node->key);
			free(node);
			return 0;
		}
		prev = node;
		node = node->next;
	}
	return 1;
}

/** 
* @brief Retrieve a value from a hashtable.
* 
* @param tbl the hashtable.
* @param key the key to retrieve.
* 
* @return if present, the stored data. Otherwise NULL.
*/
void *hashtable_get(const hashtable *tbl, const char *key)
{
	struct keypair *node;
	unsigned int hash = tbl->hasher(key) % tbl->size;

	node = tbl->nodes[hash];
	while (node) {
		if (strcmp(node->key, key) == 0)
			return node->data;
		node = node->next;
	}
	return NULL;
}

/** 
* @brief 
* 
* @param tbl
* @param size
*/
void hashtable_resize(hashtable *tbl, unsigned int size)
{
	hashtable newtbl;
	struct keypair* node;
	int i;

	newtbl.size = size;
	newtbl.hasher = tbl->hasher;
	newtbl.nodes = calloc(size, sizeof(struct keypair *));

	for(i = 0; i < tbl->size; ++i) {
		for(node = tbl->nodes[i]; node; node = node->next) {
			hashtable_insert(&newtbl, node->key, node->data);
			hashtable_remove(tbl, node->key);
		}
	}

	free(tbl->nodes);
	tbl->size = newtbl.size;
	tbl->nodes = newtbl.nodes;
}
