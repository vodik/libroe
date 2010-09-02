#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

struct ibuf {
	char *ptr;
	size_t offset, len, size;
	buf_fill fill;
	void *arg;
	int do_read;
};

static inline size_t
ibuf_fill(struct ibuf *buf)
{
	if (!buf->ptr)
		buf->ptr = malloc(buf->size);

	buf->offset = 0;
	buf->do_read = 0;
	buf->len = buf->fill(buf->ptr, buf->size, buf->arg);

	return buf->len;
}

struct ibuf *
ibuf_new(size_t size, buf_fill fill, void *arg)
{
	struct ibuf *buf = malloc(sizeof(struct ibuf));

	buf->ptr = NULL;
	buf->size = size;
	buf->len = buf->offset = 0;
	buf->do_read = 1;
	buf->fill = fill;
	buf->arg = arg;

	return buf;
}

void ibuf_free(struct ibuf *buf)
{
	if (buf->ptr)
		free(buf->ptr);
	free(buf);
}

size_t
ibuf_read(struct ibuf *buf, char *dest, size_t len)
{
	if (buf->do_read)
		ibuf_fill(buf);

	size_t ret = 0;
	while(--len) {
		*dest++ = *(buf->ptr + buf->offset++);
		++ret;

		if (buf->offset == buf->size) {
			if (ibuf_fill(buf) == 0)
				return ret;
		} else if (buf->offset == buf->len) {
			buf->do_read = 1;
			return ret;
		}
	}
	return ret;
}
