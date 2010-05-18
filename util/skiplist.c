#include "skipset.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//TODO: Make use of header

struct _skipnode {
    int value;
    skipnode** links;
};

struct _skipset_priv {
    int level;
    int height;
    int length;
};

skipnode* new_skipnode(int level, int value);

////////////////////////////////////////////////////////////////////////

float frand()
{
    static int first = 1;
    if (first) {
        srand((unsigned)time(NULL));
        first = 0;
    }
    return (float)rand() / RAND_MAX;
}

int random_level(int height)
{
    int level = 0;
    while (frand() < P && level < height)
        ++level;
    return level;
}

skipnode* new_skipnode(int level, int value)
{
    skipnode* new_node = malloc(sizeof(skipnode));
    new_node->links = calloc(level + 1, sizeof(skipnode*));

    new_node->value = value;
    return new_node;
}

skipset* new_skipset(int height)
{
    /* The header is just a normal node. We are uninterested in
     * the value as its unused (for  now?). I set it to -1 for  no
     * good reason.
     */
    skipset* new_set = malloc(sizeof(skipset));

    new_set->priv = malloc(sizeof(skipset_priv));
    new_set->priv->level = 0;
    new_set->priv->height = height;

    new_set->header = new_skipnode(height, -1);

    return new_set;
}

void free_skipset(skipset* self)
{
    skipnode* node = self->header->links[0];
    skipnode* oldnode;

    while (node != NULL) {
        oldnode = node;
        node = node->links[0];
        free(oldnode);
    }

    free(self->header);
    free(self->priv);
    free(self);
}

void skipset_print(skipset* self)
{
    skipnode* node = self->header->links[0];

    printf("[");
    while (node != NULL) {
        printf("%d", node->value);
        node = node->links[0];
        if (node != NULL)
            printf(", ");
    }
    printf("]\n");
}

void skipset_debug_print(skipset* self)
{
    int i, j;
    int* map[self->priv->height + 1];
    int pos = 0;
    skipnode* waiting[self->priv->height + 1];

    for (i = 0; i < self->priv->height + 1; ++i)
        map[i] = calloc(self->priv->length, sizeof(int*));

    for (i = self->priv->level; i >= 0; --i)
        waiting[i] = self->header->links[i];

    skipnode* node = self->header->links[0];
    while (node != NULL) {
        for (i = self->priv->level; i >= 0; --i) {
            if (waiting[i] == NULL)
                continue;
            else if (waiting[i]->value == node->value) {
                waiting[i] = node->links[i];
                map[i][pos] = 1;
            }
        }
        ++pos;
        node = node->links[0];
    }

    for (i = self->priv->level; i >= 0; --i) {
        printf("[h]");
        for (j = 0; j < self->priv->length; ++j) {
            if (map[i][j] == 1)
                printf("[*]");
            else
                printf("---");
        }
        printf("\n");
    }

    for (i = 0; i < self->priv->height + 1; ++i)
        free(map[i]);
}

void skipset_add(skipset* self, int value)
{
    //printf("TRACE: skipset_add - skipset_add value %d\n", value);
    int i;
    skipnode* node = self->header;
    skipnode* update[self->priv->height + 1];

    //memset(update, 0, sizeof(skipnode*) * (self->priv->height + 1));
    memset(update, 0, self->priv->height + 1);

    /* To skipset_add a value we first perfor m a search for  the node we are
     * skipset_adding and update all the links to the node in its place to
     * the one we are skipset_adding.
     */
    for (i = self->priv->level; i >= 0; i--) {
        while (node->links[i] != NULL && node->links[i]->value < value)
            node = node->links[i];
        update[i] = node; 
    }
    node = node->links[0];

    if (node == NULL || node->value != value) {
        int level = random_level(self->priv->height);
        //printf("TRACE: skipset_add - new node on level %d\n", level);

        /* If the new node is greater than any node already in the
         * list then the header node must be updated and the level
         * of the list must be set to the new level. 
         */
        if (level > self->priv->level) {
            for (i = self->priv->level + 1; i <= level; i++)
                update[i] = self->header;
            self->priv->level = level;
        }

        node = new_skipnode(level, value);
        ++self->priv->length;
        for (i = 0; i <= level; i++) {
            node->links[i] = update[i]->links[i];
            update[i]->links[i] = node;
        }
    }
}

void skipset_remove(skipset* self, int value)
{
    int i;
    skipnode* node = self->header;
    skipnode* update[self->priv->height + 1];

    //memset(update, 0, sizeof(skipnode*) * (self->priv->height + 1));
    memset(update, 0, self->priv->height + 1);

    /* See skipset_add
     */
    for (i = self->priv->level; i >= 0; --i) {
        while (node->links[i] != NULL && node->links[i]->value < value)
            node = node->links[i];
        update[i] = node;
    }
    node = node->links[0];

    if (node != NULL && node->value == value) {
        for (i = 0; i <= self->priv->level; ++i) {
            if (update[i]->links[i] != node)
                break;
            update[i]->links[i] = node->links[i];
        }

        free(node);
        --self->priv->length;
        while (self->priv->level > 0 && self->header->links[self->priv->level] == NULL)
            --self->priv->level;
    }
}

int skipset_contains(skipset* self, int value)
{
    int i;
    skipnode* node = self->header;

    for (i = self->priv->level; i >= 0; i--) {
        //printf("TRACE: skipset_contains - trying level %d\n", i);
        while (node->links[i] != NULL && node->links[i]->value < value)
            //printf("TRACE: skipset_contains - stepping at level %d\n", i);
            node = node->links[i];

        if (node->links[i] != NULL && node->links[i]->value == value)
            //printf("TRACE: skipset_contains - value found!\n");
            return 1;

    }
    /*node = node->links[0];

    if (node != NULL && node->value == value)
        return 1;*/
    return 0;
}

int skipset_length(skipset* self)
{
    return self->priv->length;
}
