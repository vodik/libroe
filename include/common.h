#ifndef LIBROE_UTIL_COMMON
#define LIBROE_UTIL_COMMON

typedef void (*cleanup_func)(void *ptr);
typedef unsigned int (*hashfunc)(const char *);

struct keyval_pair_t {
	int key;
	void *val;
};

#endif
