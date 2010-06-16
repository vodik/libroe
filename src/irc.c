#include <irc.h>

void irc_on_open(conn_t *conn)
{
}

int irc_on_message(conn_t *conn, const void *data)
{
	return 0;
}

void irc_on_close(conn_t *conn)
{
}

void irc_destroy(conn_t *conn)
{
}

size_t irc_write(conn_t *conn, const char *msg, size_t nbytes)
{
	return 0;
}
