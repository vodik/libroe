#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"
#include "util/sbuf.h"

enum {
	WS_STATUS_OPEN,
	WS_STATUS_CLOSED,
};

typedef struct _ws {
	int fd;
	sbuf_t *path;
	int status;

	void (*onmessage)(struct _ws *ws, const char *msg, size_t nbytes);
} ws_t;

struct ws_iface {
	int port;
	void (*onrequest)(ws_t *ws);
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, struct ws_iface *iface);

void ws_init(ws_t *ws);
void ws_free(ws_t *ws);
void ws_close(ws_t *ws);
size_t ws_send(ws_t *ws, const char *buf, size_t nbytes);

#endif
