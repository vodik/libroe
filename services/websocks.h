#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

struct ws_events_t {
	int temp;
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, int port, struct ws_events_t *events);

#endif
