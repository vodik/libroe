#include <conn.h>

void
cclose(conn_t *c)
{
	close(c->fd);
	c->status = CONN_CLOSED;
}
