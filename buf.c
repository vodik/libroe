#include "buf.h"

#include <stdlib.h>

struct buf {
	struct obuf *o;
	struct ibuf *i;
};

struct buf *
buf_new(size_t size, buf_fill fill, buf_push push, void *arg)
{
	struct buf *buf = malloc(sizeof(struct buf));
	buf->o = obuf_new(size, push, arg);
	buf->i = ibuf_new(size, fill, arg);
	return buf;
}

void
buf_free(struct buf *buf)
{
	obuf_free(buf->o);
	ibuf_free(buf->i);
}

size_t
buf_read(struct buf *buf, char *dest, size_t len)
{
	return ibuf_read(buf->i, dest, len);
}

size_t
buf_write(struct buf *buf, const char *src, size_t len)
{
	return obuf_write(buf->o, src, len);
}

void
buf_flush(struct buf *buf)
{
	obuf_flush(buf->o);
}

