#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

typedef struct {
	int fd;
} ws_t;

struct ws_ops {
	int port;
	void (*onconnect)(ws_t *ws, const char *path);
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, struct ws_ops *ops);

void ws_close(ws_t *ws);
void ws_send(ws_t *ws, const char *buf, size_t nbytes);

#endif
