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

struct ws_context_t {
	int authenticated;
};

const char message[] =
	"HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
	"Upgrade: WebSocket\r\n"
	"Connection: Upgrade\r\n"
	"WebSocket-Origin: http://localhost:11234\r\n"
	"WebSocket-Location: ws://localhost:33456/service\r\n"
	"\r\n";

void
ws_on_open(struct fd_context_t *context)
{
	struct ws_context_t *ws_context = malloc(sizeof(struct ws_context_t));
	ws_context->authenticated = 0;
	context->data = ws_context;
	context->context_gc = free;
}

int
ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	struct ws_context_t *ws_context = context->data;
	static unsigned char buf[512];

	if (!ws_context->authenticated) {
		printf(">>> got ws message!\n%s---\n", msg);
		write(context->fd, message, strlen(message));
		printf(">>> writting:\n%s---\n", message);
		ws_context->authenticated = 1;
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

void
ws_on_close(struct fd_context_t *context)
{
	printf(">>> on close!\n");
}

static struct fd_cbs_t ws_callbacks = {
	.onopen		= ws_on_open,
	.onmessage	= ws_on_message,
	.onclose	= ws_on_close,
};

void
websocks_start(struct service_t *ws, poll_mgmt_t *mgmt, struct ws_ops *ops)
{
	ws->type = SERVICE_WEBSOCKS;
	ws->fd = poll_mgmt_listen(mgmt, ops->port, &ws_callbacks, ops);
	ws->mgmt = mgmt;
	//ws->events = events;
}

/******************************************************************************/

void
ws_close(ws_t *ws)
{
	close(ws->fd);
}

size_t
ws_send(ws_t *ws, const char *buf, size_t nbytes)
{
	char _buf[nbytes + 2];
	char *b = _buf;
	int i;

	*b++ = 0x00;
	for (i = 0; i < nbytes; ++i)
		*b++ = *buf++;
	*b = (char)0xff;

	return write(ws->fd, _buf, nbytes + 2);
}
