#ifndef SMALLHTTP_WEBSOCKET
#define SMALLHTTP_WEBSOCKET

#include <conn.h>
#include <services.h>
#include <sbuf.h>

typedef struct _ws {
	conn_t *base;
	sbuf_t *path;

	void (*onmessage)(struct _ws *ws, const char *msg, size_t nbytes);
	void (*onclose)(struct _ws *ws);
} ws_t;

////////////////////////////////////////////////////////////////////////////////

/* TODO move to a private header */
void ws_on_open(conn_t *conn);
int ws_on_message(conn_t *conn, const char *msg, size_t nbytes);
void ws_on_close(conn_t *conn);

static const fd_cbs_t ws_callbacks = {
	.conn_size  = sizeof(ws_t),
	.onopen		= ws_on_open,
	.onmessage	= ws_on_message,
	.onclose	= ws_on_close,
};

////////////////////////////////////////////////////////////////////////////////

struct ws_iface {
	int port;
	void (*onopen)(ws_t *ws);
};

void ws_init(ws_t *ws);
void ws_free(ws_t *ws);
void ws_close(ws_t *ws);
size_t ws_send(ws_t *ws, const char *buf, size_t nbytes);

#endif
