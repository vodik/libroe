#include <ibuf.h>

void
ibuf_pull(ibuf_t *b)
{
	fprintf(stderr, "--- pulling\n");
	ssize_t r = b->cb(b->fd, b->buf, b->size);
	if (r < 0)
		b->err = BUF_ERR;
	else if (r == 0)
		b->err = BUF_EOF;
	else {
		fprintf(stderr, "--- ok:%d\n", r);
		b->eob = r;
		b->i = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

void
ibuf_init(ibuf_t *b, size_t size, int fd, pull_cb cb)
{
	b->fd = fd;
	b->cb = cb;
	b->err = 0;

	b->size = size;
	b->buf = malloc(size);

	ibuf_pull(b);
}

char
ibuf_getc(ibuf_t *b)
{
	if (b->i == b->eob)
		ibuf_pull(b);

	return b->err == 0 ? b->buf[b->i++] : -1;
}

int
ibuf_eof(ibuf_t *b)
{
	return b->err != 0;
}

void
ibuf_rot(ibuf *b)
{
	ibuf_pull(b);
}

size_t
ibuf_raw(ibuf *b, char **buf)
{
	*buf = b->buf + b->i;
	return b->eob - b->i;
}