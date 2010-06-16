#ifndef LIBROE_URLENCODE
#define LIBROE_URLENCODE

#include <stddef.h>
#include "hashtable.h"

#define ENCODED_LEN(len) ((len) * 3 + 1)

size_t url_encode(char *dest, const char *src);
size_t url_decode(char *dest, const char *src);

void parse_args(hashtable_t *table, const char *args);

#endif
