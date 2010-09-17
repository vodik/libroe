#include "conn.h"

#include "io.h"


struct conn_impl {
	void (*accept)();
};


//////////////////////////////////


struct conn {
	IO *io;
	unsigned refs;
};

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
