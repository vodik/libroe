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
	printf(">>> on connect!\n");
}

int ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	static int handshake = 1;
	static unsigned char buf[512];

	if (handshake) {
		printf(">>> got ws message!\n%s---\n", msg);
		write(context->fd, message, strlen(message));
		printf(">>> writting:\n%s---\n", message);
		handshake = 0;
	} else {
		char *m = (char *)msg + 1;
		m[nbytes - 2] = '\0';
		printf(">>> got ws message: \"%s\"\n", m);

		unsigned char *b = buf;
		*b++ = 0x00;
		int i = 0;
		for (i = 0; i < strlen(m); ++i)
			*b++ = m[i];
		*b = 0xff;
		i += 2;

		printf("<<< ws sent %d\n", write(context->fd, buf, i));
	}
	return 1;
}

void ws_on_close(struct fd_context_t *context)
{
	printf(">>> on close!\n");
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
