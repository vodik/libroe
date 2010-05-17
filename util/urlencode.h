#ifndef SMALLHTTP_URLENCODE
#define SMALLHTTP_URLENCODE

#include <stddef.h>

#define ENCODED_LEN(len) ((len) * 3 + 1)

size_t url_encode(char *dest, const char *src);
size_t url_decode(char *dest, const char *src);

#endif
