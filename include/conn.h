#ifndef SMALLHTTP_CONN
#define SMALLHTTP_CONN

#include <stddef.h>

enum {
	CONN_OPEN,
	CONN_CLOSED,
};

typedef void (*destroy_cb)(void *data);

typedef struct {
	int fd;
	int status;

	int ref;
	destroy_cb destroy;
} conn_t;

conn_t *conn_new(size_t size, int fd, destroy_cb destroy);
conn_t *conn_ref(conn_t *c);

void conn_close(conn_t *c);
void conn_unref(conn_t *c);

#endif
