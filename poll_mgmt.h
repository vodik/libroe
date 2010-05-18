#ifndef SMALLHTTP_POLL_MGMT_H
#define SMALLHTTP_POLL_MGMT_H

#include "util/skipset.h"

enum {
	CONN_CONNECTION,
	CONN_LISTENING,
};

struct fd_evt_t {
	int fd;
	int type;
};

typedef struct {
	int fd;
	int size;
	struct epoll_event *events;
	skipset_t store;
} poll_mgmt_t;

void poll_mgmt_start(poll_mgmt_t *mngr, int size);
void poll_mgmt_stop(poll_mgmt_t *mngr);
int poll_mgmt_listen(poll_mgmt_t *mngr, int port);
void poll_mgmt_close(poll_mgmt_t *mngr, int fd);
int poll_mgmt_poll(poll_mgmt_t *mngr, int timeout);

#endif
