#ifndef SMALLHTTP_WEBSOCKS
#define SMALLHTTP_WEBSOCKS

#include "services.h"

struct ws_events_t {
	int temp;
};

struct service_t *websocks_start(struct epoll_t *epoll, int port, struct ws_events_t *events);

#endif
