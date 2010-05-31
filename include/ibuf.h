#include <stddef.h>

enum {
	BUF_EOF = -1,
	BUF_ERR = -2,
};

typedef int (*pull_cb)(void *dat, void *buf, size_t nbytes);

typedef struct _ibuf {
	void *dat;
	int err;
	pull_cb cb;

	size_t size, eob, i;
	char *buf;
} ibuf_t;

void ibuf_init(ibuf_t *b, size_t size, int fd, pull_cb cb);
char ibuf_getc(ibuf_t *b);
int ibuf_eof(ibuf_t *b);

void ibuf_rot(ibuf *b);
size_t ibuf_raw(ibuf *b, char **buf);
