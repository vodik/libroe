#ifndef LIBROE_WEBSOCKET
#define LIBROE_WEBSOCKET

#include <stdbool.h>
#include <conn.h>
#include <services.h>

typedef struct _ws {
	conn_t base;
	request_t request;

	bool auth;

	void (*onmessage)(struct _ws *ws, const char *msg, size_t nbytes);
	void (*onclose)(struct _ws *ws);
} ws_t;

////////////////////////////////////////////////////////////////////////////////

/* TODO move to a private header */
void ws_on_open(conn_t *conn);
int ws_on_message(conn_t *conn, const void *data);
void ws_on_close(conn_t *conn);

void ws_destroy(conn_t *conn);
size_t ws_write(conn_t *conn, const char *msg, size_t nbytes);

static const fd_cbs_t ws_callbacks = {
	.cnfo = {
		.size = sizeof(ws_t),
		.destroy = ws_destroy,
		.write = ws_write,
	},

	.onopen       = ws_on_open,
	.onmessage    = ws_on_message,
	.onclose      = ws_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int port;
	void (*onopen)(ws_t *ws, request_t *request);
} ws_iface_t;

#endif
