#ifndef SMALLHTTP_UTIL_HASHTABLE
#define SMALLHTTP_UTIL_HASHTABLE

typedef struct hashtable hashtable;
typedef unsigned int (*hashfunc)(const char *);

struct keypair;

struct hashtable {
	unsigned int size;
	struct keypair **nodes;
	hashfunc hasher;
};

void hashtable_init(unsigned int size, hashfunc hasher, hashtable *tbl);
void hashtable_free(hashtable *, int freedata);

void hashtable_insert(hashtable *, const char *key, void *data);
int hashtable_remove(hashtable *, const char *key);
void *hashtable_get(const hashtable *, const char *key);

void hashtable_resize(hashtable *, unsigned int size);

#endif
