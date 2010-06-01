#ifndef SMALLHTTP_CONN
#define SMALLHTTP_CONN

enum {
	CONN_OPEN,
	CONN_CLOSED,
};

typedef struct {
	int fd;
	int status;

	int ref;
} conn_t;

conn_t *conn_new(size_t size, int fd);
conn_t *conn_ref(conn_t *c);

void conn_close(conn_t *c);
void conn_unref(conn_t *c);

#endif
