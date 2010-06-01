#include <conn.h>

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

conn_t *
conn_new(size_t size, int fd)
{
	assert(size >= sizeof(conn_t));

	conn_t *c = malloc(size);
	if (c) {
		c->fd = fd;
		c->status = CONN_OPEN;
		c->ref = 1;
	}
	return c;
}

conn_t *
conn_ref(conn_t *c)
{
	++c->ref;
	return c;
}

void
conn_unref(conn_t *c)
{
	if (--c->ref <= 0) {
		if (c->status == CONN_OPEN)
			close(c->fd);
		free(c);
	}
}

void
conn_close(conn_t *c)
{
	close(c->fd);
	c->status = CONN_CLOSED;
}
