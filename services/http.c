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
#include <assert.h>

#include <request/parser.h>
#include <util.h>

/** 
* @brief Context information needed to handle an http connection: A parser for
* the http and a response object to help generate a response.
*/
struct http_context_t {
	http_parser parser;
	http_conn conn;

	int expected_event;
	int made_request;
	char *stored_header;
};

static inline void
http_conn_init(http_conn *conn, int fd)
{
	http_response_init(&conn->response, fd);
	conn->keep_alive = CONN_KEEP_ALIVE;
}

static inline void
http_conn_free(http_conn *conn)
{
	free(conn->request.path);
	free(conn->request.version);
	http_response_end(&conn->response);
}

/** 
* @brief Helper function to generate a new context.
* 
* @param fd The file descriptor of the socket. The response needs this to be
* able to write it.
* 
* @return Returns a new context.
*/
static struct http_context_t *
http_context_new(int fd)
{
	struct http_context_t *context = malloc(sizeof(struct http_context_t));

	http_parser_init(&context->parser);
	http_conn_init(&context->conn, fd);
	context->expected_event = HTTP_DATA_METHOD;
	context->made_request = 0;
	context->stored_header = NULL;

	return context;
}

/** 
* @brief A helper function cleanup an http context. This function is meant to
* be passed into the polling framework as a "destructor" for garbage collecting
* connections.
* 
* @param data The raw pointer to the context.
*/
void
http_context_gc(void *data)
{
	struct http_context_t *context = data;

	if (context->stored_header)
		free(context->stored_header);
	http_conn_free(&context->conn);
	free(data);
}

////////////////////////////////////////////////////////////////////////////////

/** 
* @brief Function responding to an incoming http connection. Creates a context.
* 
* @param context A structure to store the context in.
*/
void
http_on_open(struct fd_context_t *context)
{
	printf("--> opening\n");
	context->data = http_context_new(context->fd);
	context->context_gc = http_context_gc;
}

static int
http_method_id(const char *msg, size_t nbytes)
{
#define CMP_METHOD(method) if (strncmp(#method, msg, nbytes) == 0) return HTTP_METHOD_##method
	CMP_METHOD(DELETE);
	CMP_METHOD(GET);
	CMP_METHOD(HEAD);
	CMP_METHOD(POST);
	CMP_METHOD(PUT);

	CMP_METHOD(CONNECT);
	CMP_METHOD(OPTIONS);
	CMP_METHOD(TRACE);
#ifdef WEBDAV
	CMP_METHOD(COPY);
	CMP_METHOD(LOCK);
	CMP_METHOD(MKCOL);
	CMP_METHOD(MOVE);
	CMP_METHOD(PROPFIND);
	CMP_METHOD(PROPPATCH);
	CMP_METHOD(UNLOCK);
#endif
	return -1;
#undef CMP_METHOD
}

/** 
* @brief Function responding to incoming data from an http connection. Incoming
* data should make up an HTTP http, which we parse and upon completion, delegate
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

int
http_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
{
	static char buf[1024]; /* TODO: formalize this */
	event_data_t data;
	int read = 0;

	struct http_context_t *http_context = context->data;
	http_parser *parser = &http_context->parser;
	http_conn *conn = &http_context->conn;

	struct http_ops *ops = context->shared;

	/* TODO: macroify keep-alive */
	if (!ops->onrequest)
		return CONN_CLOSE;

	http_parser_set_buffer(parser, msg, nbytes);

	/* if haven't, parse request */
		/* send request */

	/* IMPORTANT! DO NOT ADD BREAK STATEMENTS HERE!!! THIS IS INTENTIONAL!!!
	 * We want to fall through as each segment is processed in this order. The
	 * switch is here in case the entire header doesn't come in one message and
	 * thus have to resume parsing */
	/* TODO: check if message is incomplete */
	/* TODO: macroify */
	switch (http_context->expected_event) {
		case HTTP_DATA_METHOD:
			read = http_parser_next_event(parser, buf, 1024, &data);
			buf[read] = '\0';
			assert(data.type == HTTP_DATA_METHOD);
			http_context->expected_event = HTTP_DATA_PATH;
			conn->request.method = http_method_id(buf, read);
		case HTTP_DATA_PATH:
			read = http_parser_next_event(parser, buf, 1024, &data);
			assert(data.type == HTTP_DATA_PATH);
			http_context->expected_event = HTTP_DATA_VERSION;
			conn->request.path = strndup(buf, read);
		/*case HTTP_DATA_QUERY:
		case HTTP_DATA_FRAGMENT:*/
		case HTTP_DATA_VERSION:
			read = http_parser_next_event(parser, buf, 1024, &data);
			assert(data.type == HTTP_DATA_VERSION);
			http_context->expected_event = HTTP_DATA_HEADER;
			conn->request.version = strndup(buf, read);
	}

	if (!http_context->made_request) {
		ops->onrequest(conn);

		if (!conn->makeresponse)
			return CONN_CLOSE;
		http_context->made_request = 1;
	}

	/* if interested in headers, send headers */
	while ((read = http_parser_next_event(parser, buf, 1024, &data)) > 0) {
		switch (http_context->expected_event) {
			case HTTP_DATA_HEADER:
				assert(data.type == HTTP_DATA_HEADER);
				http_context->expected_event = HTTP_DATA_FIELD;
				http_context->stored_header = strndup(buf, read);
				break;
			case HTTP_DATA_FIELD:
				assert(data.type == HTTP_DATA_FIELD);
				http_context->expected_event = HTTP_DATA_HEADER;
				buf[read] = '\0';

				/* TODO: internal handling of relavant headers here */
				if (strcmp(http_context->stored_header, "Connection") == 0) {
					printf("-- have connection header\n");
					if (strcmp(buf, "close") == 0) {
						printf("-- connection will close\n");
						conn->keep_alive = CONN_CLOSE;
					}
				}

				if (conn->onheader)
					conn->onheader(conn, http_context->stored_header, buf);
				
				free(http_context->stored_header);
				http_context->stored_header = 0;
				break;
		}
	}

	/* finally, request a body */
	conn->makeresponse(conn);
	return conn->keep_alive;
}
/******************************************************************************/
	/*struct http_events_t *cb = context->shared;

	int read;

	int keep_alive = 1;

	printf("here - bytes: %d/%d\n%s---\n", nbytes, strlen(msg), msg);
	http_parser_set_buffer(parser, msg, nbytes);

	//struct http_request_t request;

	read = http_parser_next_event(parser, buf, 1024, &data);
	assert(data.type == HTTP_DATA_METHOD);
	buf[read] = '\0';
	int method = http_method_id(buf, read);

	read = http_parser_next_event(parser, buf, 1024, &data);
	assert(data.type == HTTP_DATA_PATH);
	buf[read] = '\0';

	keep_alive = cb->cbs[http_context->request.method](&http_context->request, &http_context->response);

	free(http_context->request.url);*/
	
	/*while ((read = http_parser_next_event(parser, buf, 1024, &data)) > 0) {
		if (data.type == HTTP_DATA_METHOD) {
			send_to = http_method_id(buf, read);
		}
		buf[read] = '\0';*/
	//}
	/*printf("--- returned: %d\n", read);
	return keep_alive;*/
/******************************************************************************/

/** 
* @brief Function responding to a connection closing. Currently a nop.
* NOTE: do not use this function to cleanup context data. A destructor is
* already specified elsewhere.
* 
* @param context A structure storing the context.
*/
void
http_on_close(struct fd_context_t *context)
{
}

/** 
* @brief The callbacks specific to handling HTTP https.
*/
static struct fd_cbs_t http_callbacks = {
	.onopen		= http_on_open,
	.onmessage	= http_on_message,
	.onclose	= http_on_close,
};

/** 
* @brief Start the HTTP service. This sets the provided polling manager to
* start listening for HTTP https and handle them.
* 
* @param http The service data structure.
* @param mgmt A pointer to the polling manager to manage the connections.
* @param port The port to listen on.
* @param events HTTP event callbacks to receive GET, POST, etc. messages
*/
void
http_start(struct service_t *http, poll_mgmt_t *mgmt, struct http_ops *ops)
{
	http->type = SERVICE_HTTP;
	http->fd = poll_mgmt_listen(mgmt, ops->port, &http_callbacks, ops);//, &http_callbacks, http);
	http->mgmt = mgmt;
	//http->cb = cb;
}
