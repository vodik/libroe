#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include "services.h"
#include <response/response.h>
#include <request/parser.h>

#define MAX_PATH_LENGTH		512
#define MAX_QUERY_LENGTH	512
#define MAX_FRAGMENT_LENGTH	512

struct http_request_t {
	//int method;
	char *url;
	/*char query[MAX_QUERY_LENGTH];
	char fragment[MAX_FRAGMENT_LENGTH];*/
};

typedef int (*request_cb)(struct http_request_t *request, http_response *response);

struct http_events_t {
	int port;
	void (*on_request)(http_conn *conn, request_data *data);
	void (*on_headers)(http_conn *conn, const char *header, const char *field);
};

void http_start(struct service_t *http, poll_mgmt_t *mgmt, int port, struct http_events_t *events);

#endif
