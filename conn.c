#include "conn.h"

#include "io.h"
#include "conn_ref.h"

struct conn *
conn_new_fd(int fd)
{
	struct conn *conn = malloc(sizeof(struct conn));
	conn->io = io_new_fd(fd);
	conn->refs = 1;

	return conn;
}

struct conn *
conn_ref(struct conn *conn)
{
	++conn->refs;
	return conn;
}

void
conn_close(struct conn *conn)
{
	if (--conn->refs == 0) {
		io_close(conn->io);
		free(conn);
	}
}
