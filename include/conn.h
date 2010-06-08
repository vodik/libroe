#ifndef SMALLHTTP_CONN
#define SMALLHTTP_CONN

#include <stddef.h>

typedef void (*destroy_cb)(void *data);

typedef struct {
	int fd;
	int ref;
	destroy_cb destroy;
} conn_t;

conn_t *conn_new(size_t size, int fd, destroy_cb destroy);
conn_t *conn_ref(conn_t *c);
void conn_close(conn_t *c);

size_t conn_write(conn_t *c, const char *msg, size_t bytes);

#endif
