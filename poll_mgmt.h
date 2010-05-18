#ifndef SMALLHTTP_POLL_MGMT_H
#define SMALLHTTP_POLL_MGMT_H

#include <stddef.h>
#include "util/skipset.h"

enum {
	CONN_CONNECTION,
	CONN_LISTENING,
};

typedef void (*context_free_cb)(void *data);

struct fd_context_t {
	void *data;
	context_free_cb context_free;
};

typedef void (*onopen_cb)(struct fd_context_t *context);
typedef void (*onmessage_cb)(struct fd_context_t *context, const char *msg, size_t nbytes);
typedef void (*onclose_cb)(struct fd_context_t *context);

struct fd_cbs_t {
	onopen_cb onopen;
	onmessage_cb onmessage;
	onclose_cb onclose;
};

struct fd_evt_t {
	int fd;
	int type;
	struct fd_cbs_t *cbs;
	struct fd_context_t context;
};

typedef struct {
	int fd;
	int size;
	struct epoll_event *events;
	skipset_t store;
} poll_mgmt_t;

void poll_mgmt_start(poll_mgmt_t *mngr, int size);
void poll_mgmt_stop(poll_mgmt_t *mngr);
int poll_mgmt_listen(poll_mgmt_t *mngr, int port, struct fd_cbs_t *cbs);
void poll_mgmt_close(poll_mgmt_t *mngr, int fd);
int poll_mgmt_poll(poll_mgmt_t *mngr, int timeout);

#endif
