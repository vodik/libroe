#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

struct ws_events_t {
	int temp;
};

struct service_t *websocks_start(poll_mgmt_t *mgmt, int port, struct ws_events_t *events);

#endif
