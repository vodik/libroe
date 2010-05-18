#ifndef SMALLHTTP_SKIPSET_H
#define SMALLHTTP_SKIPSET_H

#define P 0.5

struct keyval_pair_t {
	int key;
	void *val;
};

struct skipnode_t {
	struct keyval_pair_t keypair;
	struct skipnode_t** links;
};

typedef struct {
	struct skipnode_t* header;
	int level;
	int height;
	int length;
} skipset_t;

typedef void (*cleanup_func)(void *ptr);

void skipset_init(skipset_t *set, int height);
void skipset_cleanup(skipset_t *set, cleanup_func clean);

void skipset_add(skipset_t *set, int key, void *data);
void *skipset_get(skipset_t *set, int key);
void *skipset_remove(skipset_t *set, int key);
int skipset_has(skipset_t *set, int key);
int skipset_length(skipset_t *set);

#endif
