#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include "services.h"
#include <response/response.h>
#include <request/parser.h>

typedef struct request_data {
	int method;
	char *url, *query, *frag;
};

typedef struct {
	request_data request;
	http_response response;

	int keep_alive;

	void (*onheaders)(http_conn *conn, const char *header, const char *field);
	void (*makeresponse)(http_conn *conn);
} http_conn;

struct http_ops {
	int port;
	void (*onrequest)(http_conn *conn);
};

void http_start(struct service_t *http, poll_mgmt_t *mgmt, struct http_ops *events);

#endif
