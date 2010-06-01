#ifndef SMALLHTTP_WEBSOCKET
#define SMALLHTTP_WEBSOCKET

#include <conn.h>
#include <services.h>
#include <sbuf.h>

/* TODO move to a private header */
void ws_on_open(struct fd_context_t *context);
int ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes);
void ws_on_close(struct fd_context_t *context);

static const fd_cbs_t ws_callbacks = {
	.onopen		= ws_on_open,
	.onmessage	= ws_on_message,
	.onclose	= ws_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct _ws {
	conn_t *base;
	sbuf_t *path;

	void (*onmessage)(struct _ws *ws, const char *msg, size_t nbytes);
	void (*onclose)(struct _ws *ws);
} ws_t;

struct ws_iface {
	int port;
	void (*onopen)(ws_t *ws);
};

void ws_init(ws_t *ws);
void ws_free(ws_t *ws);
void ws_close(ws_t *ws);
size_t ws_send(ws_t *ws, const char *buf, size_t nbytes);

#endif
