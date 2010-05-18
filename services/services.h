#ifndef SMALLHTTP_SERVICES
#define SMALLHTTP_SERVICES

#include "poll_mgmt.h"

enum {
	SERVICE_HTTP,
	SERVICE_WEBSOCKS,
};

struct service_t {
	int type;
	int fd;
	poll_mgmt_t *mgmt;
	void *events;
};

void service_end(poll_mgmt_t *mgmt, struct service_t *service);

#endif
