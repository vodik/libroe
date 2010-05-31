#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include <services.h>
#include <response.h>
#include <parser.h>

typedef struct {
	int method;
	char *path;//, *query, *frag;
	char *version;
} request_data;

typedef struct http_conn {
	request_data request;
	http_response response;

	int keep_alive;

	void (*onheader)(struct http_conn *conn, const char *header, const char *field);
	void (*makeresponse)(struct http_conn *conn);
} http_conn;

struct http_iface {
	int port;
	void (*onrequest)(http_conn *conn);
};

void http_start(struct service_t *http, poll_mgmt_t *mgmt, struct http_iface *events);

#endif
