#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

typedef struct {
	int fd;
	char *path;
	int status;
} ws_t;

struct ws_ops {
	int port;
	void (*onrequest)(ws_t *ws);
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, struct ws_ops *ops);

void ws_close(ws_t *ws);
size_t ws_send(ws_t *ws, const char *buf, size_t nbytes);

#endif
