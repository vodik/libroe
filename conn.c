#include "conn.h"

#include <stdlib.h>

#include "io.h"
#include "conn_ref.h"

/*struct conn *
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
}*/

struct conn *
conn_new(struct service *service, IO *io)
{
	struct conn *conn = malloc(sizeof(struct conn));
	conn->io = io;
	conn->service = service;
	conn->refs = 1;

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

struct request *
conn_request(struct conn *conn)
{
	return conn->request;
}
