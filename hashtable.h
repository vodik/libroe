#ifndef LIBROE_UTIL_HASHTABLE
#define LIBROE_UTIL_HASHTABLE

#include "common.h"

struct hashnode_t {
	char *key;
	void *val;
	struct hashnode_t *next;
};

typedef struct {
	unsigned int size;
	struct hashnode_t **nodes;
	hashfunc hasher;
} hashtable_t;

void hashtable_init(hashtable_t *table, unsigned int size, hashfunc hasher);
void hashtable_cleanup(hashtable_t *table, cleanup_func clean);

void hashtable_add(hashtable_t *table, const char *key, void *data);
void *hashtable_get(const hashtable_t *table, const char *key);
void *hashtable_remove(hashtable_t *table, const char *key);

#endif
