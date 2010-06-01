#ifndef SMALLHTTP_IBUF
#define SMALLHTTP_IBUF

#include <stddef.h>

enum {
	BUF_EOF = -1,
	BUF_ERR = -2,
	BUF_TIMEOUT = -3,
};

typedef int (*pull_cb)(void *dat, void *buf, size_t nbytes);

typedef struct _ibuf {
	void *dat;
	int err;
	pull_cb cb;

	size_t size, eob, i;
	char *buf;
} ibuf_t;

void ibuf_init(ibuf_t *b, size_t size, void *dat, pull_cb cb);
char ibuf_getc(ibuf_t *b);
int ibuf_eof(ibuf_t *b);

void ibuf_rot(ibuf_t *b);
size_t ibuf_raw(ibuf_t *b, char **buf);

#endif
