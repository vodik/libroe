#ifndef SMALLHTTP_SERVICES
#define SMALLHTTP_SERVICES

#include <poll_mgmt.h>

/*enum {
	CONN_CLOSE,
	CONN_KEEP_ALIVE,
};*/

typedef struct {
	int fd;
	poll_mgmt_t *mgmt;
	//void *events;
} service_t;

int service_start(service_t *service, const char *name, poll_mgmt_t *mgmt, int port, void *iface);
//void service_end(poll_mgmt_t *mgmt, struct service_t *service);

#endif
