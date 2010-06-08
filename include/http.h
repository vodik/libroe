#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include <services.h>
#include <response.h>
#include <parser.h>
#include <conn.h>
#include <hashtable.h>

typedef struct {
	char *method;
	char *path;
	char *version;
	hashtable_t headers;
} request_t;

typedef struct _http {
	conn_t base;
	int keep_alive;

	request_t request;
	response_t response;
} http_t;

////////////////////////////////////////////////////////////////////////////////

/* TODO move to a private header */
void http_on_open(conn_t *conn);
int http_on_message(conn_t *conn, void *data);
void http_on_close(conn_t *conn);

void http_destroy(conn_t *conn);
size_t http_write(conn_t *conn, const char *msg, size_t nbytes);

static const fd_cbs_t http_callbacks = {
	.conn_size    = sizeof(http_t),
	.conn_destroy = http_destroy,
	.conn_write   = http_write,

	.onopen       = http_on_open,
	.onmessage    = http_on_message,
	.onclose      = http_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int port;
	void (*onrequest)(http_t *conn, request_t *request, response_t *response);
} http_iface_t;

#endif
