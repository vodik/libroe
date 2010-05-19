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

/** 
* @brief Context information needed to handle an http connection: A parser for
* the request and a response object to help generate a response.
*/
struct http_context_t {
	http_parser parser;
	http_response response;
};

/** 
* @brief Helper function to generate a new context.
* 
* @param fd The file descriptor of the socket. The response needs this to be
* able to write it.
* 
* @return Returns a new context.
*/
static struct http_context_t *http_context_new(int fd)
{
	struct http_context_t *context = malloc(sizeof(struct http_context_t));
	http_parser_init(&context->parser);
	http_response_init(&context->response, fd);
	return context;
}

/** 
* @brief A helper function cleanup an http context. This function is meant to
* be passed into the polling framework as a "destructor" for garbage collecting
* connections.
* 
* @param data The raw pointer to the context.
*/
void http_context_free(void *data)
{
	struct http_context_t *context = data;
	http_parser_free(&context->parser);
	http_response_end(&context->response);
	free(data);
}

////////////////////////////////////////////////////////////////////////////////

/** 
* @brief Function responding to an incoming http connection. Creates a context.
* 
* @param context A structure to store the context in.
*/
void http_on_open(struct fd_context_t *context)
{
	printf("--> opening\n");
	context->data = http_context_new(context->fd);
	context->context_free = http_context_free;
}

/** 
* @brief Function responding to incoming data from an http connection. Incoming
* data should make up an HTTP request, which we parse and upon completion, delegate
* further to http service specific callbacks in response to different HTTP methods
* such as GET or POST.
* 
* @param context A structure storing the context.
* @param msg The incoming data.
* @param nbytes The length of the incoming data. Max size specified in config.h in
* the macro POLL_MGMT_BUFF_SIZE.
* 
* @return A keep-alive state. The polling subsystem closes the connection if this is
* not set.
*/
int http_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	struct http_context_t *http_context = context->data;
	http_parser *parser = &http_context->parser;
	http_response *response = &http_context->response;

	printf("here - bytes: %d/%d\n%s---\n", nbytes, strlen(msg), msg);
	http_parser_read(parser, msg, nbytes);
	printf("--> reading done\n");

	const http_request *request;
	int result = 0;

	if ((request = http_parser_done(parser))) {
		printf("%d: got request\n", context->fd);
		struct http_events_t *events = context->shared;
		if (events) {
			switch (request->method) {
				case HTTP_GET:
					if (events->GET)
						result = events->GET(request, response);
					break;
				case HTTP_POST:
					if (events->POST)
						result = events->POST(request, response);
					break;
				case HTTP_PUT:
					if (events->PUT)
						result = events->PUT(request, response);
					break;
			}
		}
		return result;
	}
	return 1;
}

/** 
* @brief Function responding to a connection closing. Currently a nop.
* NOTE: do not use this function to cleanup context data. A destructor is
* already specified elsewhere.
* 
* @param context A structure storing the context.
*/
void http_on_close(struct fd_context_t *context)
{
}

/** 
* @brief The callbacks specific to handling HTTP requests.
*/
static struct fd_cbs_t http_callbacks = {
	.onopen		= http_on_open,
	.onmessage	= http_on_message,
	.onclose	= http_on_close,
};

/** 
* @brief Start the HTTP service. This sets the provided polling manager to
* start listening for HTTP requests and handle them.
* 
* @param http The service data structure.
* @param mgmt A pointer to the polling manager to manage the connections.
* @param port The port to listen on.
* @param events HTTP event callbacks to receive GET, POST, etc. messages
*/
void http_start(struct service_t *http, poll_mgmt_t *mgmt, int port, struct http_events_t *events)
{
	http->type = SERVICE_HTTP;
	http->fd = poll_mgmt_listen(mgmt, port, &http_callbacks, events);//, &http_callbacks, http);
	http->mgmt = mgmt;
	http->events = events;
}
