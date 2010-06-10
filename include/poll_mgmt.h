#ifndef SMALLHTTP_POLL_MGMT_H
#define SMALLHTTP_POLL_MGMT_H

#include <stddef.h>
#include <conn.h>
#include <skipset.h>

enum {
	CONN_CONNECTION,
	CONN_LISTENING,
};

typedef void (*onopen_cb)(conn_t *conn);
typedef int (*onmessage_cb)(conn_t *conn, const void *data);
typedef void (*onclose_cb)(conn_t *conn);

typedef struct {
	conn_info_t cnfo;

	onopen_cb onopen;
	onmessage_cb onmessage;
	onclose_cb onclose;
} fd_cbs_t;

typedef struct {
	int fd;
	int size;
	struct epoll_event *events;
	skipset_t store;
} poll_mgmt_t;

void poll_mgmt_start(poll_mgmt_t *mngr, int size);
void poll_mgmt_stop(poll_mgmt_t *mngr);
int poll_mgmt_listen(poll_mgmt_t *mngr, int port, const fd_cbs_t *cbs, const void *shared);
void poll_mgmt_close(poll_mgmt_t *mngr, int fd);
int poll_mgmt_poll(poll_mgmt_t *mngr, int timeout);

#endif
