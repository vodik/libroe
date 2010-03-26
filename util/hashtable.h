#ifndef SMALLHTTP_UTIL_HASHTABLE
#define SMALLHTTP_UTIL_HASHTABLE

typedef struct hashashtable hashashtable;
typedef unsigned int (*hashfunc)(const char *);

struct keypair;

struct hashashtable {
	unsigned int size;
	struct keypair **nodes;
	hashfunc hasher;
};

void hashtable_init(unsigned int size, hashfunc hasher, hashashtable *tbl);
void hashtable_free(hashashtable *, int freedata);

void hashtable_insert(hashashtable *, const char *key, void *data);
int hashtable_remove(hashashtable *, const char *key);
void *hashtable_get(const hashashtable *, const char *key);

void hashtable_resize(hashashtable *, unsigned int size);

#endif
