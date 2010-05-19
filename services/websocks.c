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

const char message[] =
	"HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
	"Upgrade: WebSocket\r\n"
	"Connection: Upgrade\r\n"
	"WebSocket-Origin: http://localhost:44567\r\n"
	"WebSocket-Location: ws://localhost:33456/service\r\n"
	"\r\n";

void ws_on_open(struct fd_context_t *context)
{
}

int ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	printf(">>> got ws message!\n%s---\n", msg);
	write(context->fd, message, strlen(message));
	printf(">>> writting:\n%s---\n", message);

	char buf[14];
	buf[0] = 0x00;
	strcpy(buf + 1, "hello world");
	buf[11] = (char)0xff;
	write(context->fd, buf, 11);
	return 1;
}

void ws_on_close(struct fd_context_t *context)
{
}

static struct fd_cbs_t ws_callbacks = {
	.onopen		= ws_on_open,
	.onmessage	= ws_on_message,
	.onclose	= ws_on_close,
};

void websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, int port, struct ws_events_t *events)
{
	ws->type = SERVICE_WEBSOCKS;
	ws->fd = poll_mgmt_listen(mgmt, port, &ws_callbacks, events);//, &ws_callbacks, ws);
	ws->mgmt = mgmt;
	ws->events = events;
}
