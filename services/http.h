#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include "services.h"
#include <request/request.h>
#include <response/response.h>

typedef int (*request_cb)(const http_request const *request, http_response *response);

struct http_events_t {
	request_cb GET;
	request_cb PUT;
	request_cb POST;
};

void http_start(struct service_t *http, poll_mgmt_t *mgmt, int port, struct http_events_t *events);

#endif
