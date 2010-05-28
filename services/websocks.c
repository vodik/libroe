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
#include <assert.h>

#include <request/parser.h>

#include "util.h"

struct ws_context_t {
	http_parser parser;
	ws_t ws;

	int auth;
	int expected_event;
	char *path;
	char *header;
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
	http_parser_init(&ws_context->parser);

	ws_context->auth = 0;
	ws_context->ws.fd = context->fd;

	context->data = ws_context;
	context->context_gc = free;
}

int
ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	static char buf[1024];
	event_data_t data;
	int read = 0;

	struct ws_context_t *ws_context = context->data;
	http_parser *parser = &ws_context->parser;
	ws_t *ws = &ws_context->ws;

	struct ws_ops *ops = context->shared;

	if (!ops->onrequest)
		return CONN_CLOSE;

	http_parser_set_buffer(parser, msg, nbytes);

	if (!ws_context->auth) {
		switch (ws_context->expected_event) {
			case HTTP_DATA_METHOD:
				read = http_parser_next_event(parser, buf, 1024, &data);
				buf[read] = '\0';
				assert(data.type == HTTP_DATA_METHOD);
				ws_context->expected_event = HTTP_DATA_PATH;
				if (strncmp(buf, "GET", read) != 0) {
					fprintf(stderr, "--- WEBSOCKET CAN ONLY GET\n");
					return CONN_CLOSE;
				}
			case HTTP_DATA_PATH:
				read = http_parser_next_event(parser, buf, 1024, &data);
				assert(data.type == HTTP_DATA_PATH);
				ws_context->expected_event = HTTP_DATA_VERSION;
				ws->path = strndup(buf, read);
			case HTTP_DATA_VERSION:
				read = http_parser_next_event(parser, buf, 1024, &data);
				assert(data.type == HTTP_DATA_VERSION);
				ws_context->expected_event = HTTP_DATA_HEADER;
				/* use version info? */
			case HTTP_DATA_HEADER:
				ops->onrequest(ws);
				ws_context->auth = 1;
		}
	}

	return CONN_KEEP_ALIVE;
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
