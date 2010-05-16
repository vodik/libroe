#ifndef SMALLHTTP_SERVICES
#define SMALLHTTP_SERVICES

#include "socks.h"

enum {
	SERVICE_HTTP,
	SERVICE_WEBSOCKS,
};

struct service_t {
	int type;
	int fd;
	struct epoll_t *epoll;
	void *events;
};

#endif
