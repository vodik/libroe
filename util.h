#ifndef LIBROE2_UTIL
#define LIBROE2_UTIL

#define LENGTH(X)                 (sizeof X / sizeof X[0] )

#include <stddef.h>

void die(const char *err, ...);
int itoa(int n, char s[], size_t len, int radix);

#endif
