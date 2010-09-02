#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

struct obuf {
	char *ptr;
	size_t offset, size;
	buf_push push;
	void *arg;
	int dirty;
};

struct obuf *
obuf_new(size_t size, buf_push push, void *arg)
{
	struct obuf *buf = malloc(sizeof(struct obuf));

	buf->ptr = NULL;
	buf->size = size;
	buf->offset = 0;
	buf->push = push;
	buf->arg = arg;
	buf->dirty = 1;

	return buf;
}

void obuf_free(struct obuf *buf)
{
	if (buf->dirty)
		obuf_flush(buf);
	if (buf->ptr)
		free(buf->ptr);
	free(buf);
}

size_t
obuf_write(struct obuf *buf, const char *src, size_t len)
{
	if (!buf->ptr)
		buf->ptr = malloc(buf->size);

	size_t ret = 0;
	while(--len) {
		*(buf->ptr + buf->offset++) = *src++;
		++ret;

		if (buf->offset == buf->size)
			obuf_flush(buf);
	}
	return ret;
}

void
obuf_flush(struct obuf *buf)
{
	if (buf->ptr) {
		buf->push(buf->ptr, buf->offset, buf->arg);
		buf->offset = 0;
	}
}
