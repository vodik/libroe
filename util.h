#ifndef SMALLHTTP_UTIL
#define SMALLHTTP_UTIL

#define LENGTH(X)                 (sizeof X / sizeof X[0] )

void die(const char *err, ...);

#endif
