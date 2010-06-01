#ifndef SMALLHTTP_CONN
#define SMALLHTTP_CONN

enum {
	CONN_OPEN,
	CONN_CLOSED,
};

typedef struct {
	int fd;
	int status;
} conn_t;

void cclose(conn_t *c);

#endif
