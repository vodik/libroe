#include <conn.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

conn_t *
conn_new(size_t size, int fd, destroy_cb destroy)
{
	assert(size >= sizeof(conn_t));

	conn_t *c = malloc(size);
	if (c) {
		c->fd = fd;
		c->status = CONN_OPEN;
		c->ref = 1;
		c->destroy = destroy;
	}
	return c;
}

conn_t *
conn_ref(conn_t *c)
{
	printf("/// conn ref: %d --> %d\n", c->fd, c->ref + 1);
	++c->ref;
	return c;
}

void
conn_unref(conn_t *c)
{
	printf("/// conn unref: %d --> %d\n", c->fd, c->ref - 1);
	if (--c->ref <= 0) {
		if (c->status == CONN_OPEN) {
			printf("/// conn closing fd\n");
			close(c->fd);
		}

		if (c->destroy) {
			printf("/// conn calling destroy\n");
			c->destroy(c);
		}
		free(c);
	}
}

void
conn_close(conn_t *c)
{
	close(c->fd);
	c->status = CONN_CLOSED;
}
