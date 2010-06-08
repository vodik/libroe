#ifndef SMALLHTTP_CONN
#define SMALLHTTP_CONN

#include <stddef.h>

typedef struct _conn conn_t;

typedef void (*destroy_cb)(struct _conn *c);
typedef size_t (*write_cb)(struct _conn *c, const char *msg, size_t nbytes);

struct _conn {
	int fd;
	int ref;

	/* interface */
	write_cb write;
	destroy_cb destroy;
};

conn_t *conn_new(size_t size, int fd, write_cb write, destroy_cb destroy);
conn_t *conn_ref(conn_t *c);
void conn_close(conn_t *c);

size_t conn_write(conn_t *c, const char *msg, size_t bytes);

#endif
