#include "websocks.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "util.h"

void ws_on_open(struct fd_context_t *context)
{
}

int ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	return 0;
}

void ws_on_close(struct fd_context_t *context)
{
}

static struct fd_cbs_t ws_callbacks = {
	.onopen		= ws_on_open,
	.onmessage	= ws_on_message,
	.onclose	= ws_on_close,
};

struct service_t *websocks_start(poll_mgmt_t *mgmt, int port, struct ws_events_t *events)
{
	struct service_t *ws = malloc(sizeof(struct service_t));
	ws->type = SERVICE_WEBSOCKS;
	ws->fd = poll_mgmt_listen(mgmt, port, &ws_callbacks, events);//, &ws_callbacks, ws);
	ws->mgmt = mgmt;
	ws->events = events;
	return ws;
}
