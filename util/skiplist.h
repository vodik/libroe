#ifndef SMALLHTTP_SKIPSET_H
#define SMALLHTTP_SKIPSET_H

#define P 0.5

typedef struct _skipnode skipnode;
typedef struct _skipset skipset;
typedef struct _skipset_priv skipset_priv;

struct _skipset
{
    skipnode* header;
    skipset_priv* priv;
};

skipset* new_skipset(int height);
void free_skipset();

void skipset_print(skipset* set);
void skipset_debug_print(skipset* set);

void skipset_add(skipset* set, int value);
void skipset_remove(skipset* set, int value);
int skipset_contains(skipset* set, int value);
int skipset_length(skipset* set);

#endif
