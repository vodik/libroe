#include "skipset.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

static inline float
frand()
{
	static int first = 1;
	if (first) {
		srand((unsigned)time(NULL));
		first = 0;
	}
	return (float)rand() / RAND_MAX;
}

static inline int
rand_level(int height)
{
	int level = 0;
	while (frand() < P && level < height)
		++level;
	return level;
}

static struct skipnode_t *
skipnode_new(int level, int key, void *val)
{
	struct skipnode_t *node = malloc(sizeof(struct  skipnode_t));
	node->links = calloc(level + 1, sizeof(struct skipnode_t*));
	node->keypair.key = key;
	node->keypair.val = val;
	return node;
}

void
skipset_init(skipset_t *set, int height)
{
	set->level = 0;
	set->height = height;
	set->header = skipnode_new(height, -1, NULL);
}

void
skipset_cleanup(skipset_t *set, cleanup_func clean)
{
	struct skipnode_t *prev, *node = set->header->links[0];

	while (node != NULL) {
		prev = node;
		node = node->links[0];
		if (clean)
			clean(prev->keypair.val);
		free(prev->links);
		free(prev);
	}
	free(set->header->links);
	free(set->header);
}

void
skipset_add(skipset_t *set, int key, void *data)
{
	int i;
	struct skipnode_t *node = set->header;
	struct skipnode_t *update[set->height + 1];

	memset(update, 0, set->height + 1);

	for (i = set->level; i >= 0; i--) {
		while (node->links[i] != NULL && node->links[i]->keypair.key < key)
			node = node->links[i];
		update[i] = node;
	}
	node = node->links[0];

	if (node == NULL || node->keypair.key != key) {
		int level = rand_level(set->height);

		if (level > set->level) {
			for (i = set->level + 1; i <= level; i++)
				update[i] = set->header;
			set->level = level;
		}

		node = skipnode_new(level, key, data);
		++set->length;
		for (i = 0; i <= level; i++) {
			node->links[i] = update[i]->links[i];
			update[i]->links[i] = node;
		}
	}
}

void *
skipset_remove(skipset_t *set, int key)
{
	int i;
	struct skipnode_t *node = set->header;
	struct skipnode_t *update[set->height + 1];
	void *ret = NULL;

	memset(update, 0, set->height + 1);

	for (i = set->level; i >= 0; --i) {
		while (node->links[i] != NULL && node->links[i]->keypair.key < key)
			node = node->links[i];
		update[i] = node;
	}
	node = node->links[0];

	if (node != NULL && node->keypair.key == key) {
		for (i = 0; i <= set->level; ++i) {
			if (update[i]->links[i] != node)
				break;
			update[i]->links[i] = node->links[i];
		}

		ret = node->keypair.val;
		free(node->links);
		free(node);
		--set->length;
		while (set->level > 0 && set->header->links[set->level] == NULL)
			--set->level;
	}
	return ret;
}

int
skipset_has(skipset_t *set, int key)
{
	int i;
	struct skipnode_t *node = set->header;

	for (i = set->level; i >= 0; i--) {
		while (node->links[i] != NULL && node->links[i]->keypair.key < key)
			node = node->links[i];
		if (node->links[i] != NULL && node->links[i]->keypair.key == key)
			return 1;

	}
	return 0;
}

int
skipset_length(skipset_t *set)
{
	return set->length;
}
