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

typedef struct {
	const char *name;
	fd_cbs_t *cbs;
} srv_descpt_t;

void service_start(service_t *ws, poll_mgmt_t *mgmt, void *iface);
//void service_end(poll_mgmt_t *mgmt, struct service_t *service);

#endif
