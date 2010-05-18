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

void *ws_on_connection(int fd, void *arg)
{
	return NULL;
}

int ws_on_recv(int fd, void *context, void *arg)
{
	return 1;
}

void ws_on_disconnection(void *context, void *arg)
{
}

/*static struct fdcbs_t ws_callbacks = {
	.onconn		= ws_on_connection,
	.onrecv		= ws_on_recv,
	.ondisconn	= ws_on_disconnection,
};*/

struct service_t *websocks_start(poll_mgmt_t *mgmt, int port, struct ws_events_t *events)
{
	struct service_t *ws = malloc(sizeof(struct service_t));
	ws->type = SERVICE_WEBSOCKS;
	//ws->fd = epoll_listen(epoll, port, &ws_callbacks, ws);
	ws->mgmt = mgmt;
	ws->events = events;
	return ws;
}
