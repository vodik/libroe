#include "http.h"

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

#include <request/parser.h>
#include <util.h>

struct http_context_t {
	http_parser parser;
	http_response response;
};

void *http_on_connection(int fd, void *arg)
{
	struct http_context_t *context = malloc(sizeof(struct http_context_t));
	http_parser_init(&context->parser, fd);
	http_response_init(&context->response, fd);
	return context;
}

int http_on_recv(int fd, void *context, void *arg)
{
	struct http_context_t *ctext = context;
	http_parser *parser = &ctext->parser;
	http_response *response = &ctext->response;
	int result = 1;

	struct service_t *service = arg;

	char buf[BUFSIZ];
	ssize_t r = 1;
	const http_request const *request;

	while (r > 0) {
		r = read(fd, buf, BUFSIZ);
		http_parser_read(parser, buf, r);
	}
	request = http_parser_done(parser);

	if (request) {
		struct http_events_t *events = service->events;
		if (events) {
			switch (request->method) {
				case HTTP_GET:
					if (events->GET)
						result = events->GET(request, response);
				case HTTP_POST:
					if (events->POST)
						result = events->POST(request, response);
				case HTTP_PUT:
					if (events->PUT)
						result = events->PUT(request, response);
			}
		}
		return result;
	}
	return 0;
}

void http_on_disconnection(void *context, void *arg)
{
	struct http_context_t *ctext = context;
	http_parser_free(&ctext->parser);
	http_response_end(&ctext->response);
	free(ctext);
}

static struct fdcbs_t http_callbacks = {
	.onconn		= http_on_connection,
	.onrecv		= http_on_recv,
	.ondisconn	= http_on_disconnection,
};

struct service_t *http_start(poll_mgmt_t *mgmt, int port, struct http_events_t *events)
{
	struct service_t *http = malloc(sizeof(struct service_t));
	http->type = SERVICE_HTTP;
	http->fd = poll_mgmt_listen(mgmt, port);//, &http_callbacks, http);
	http->mgmt = mgmt;
	http->events = events;
	return http;
}
