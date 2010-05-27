#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

struct ws_ops {
	int port;
	//void (*onrequest)(http_conn *conn);
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, struct ws_ops *ops);

#endif
