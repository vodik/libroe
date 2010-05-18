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

struct service_t *http_start(poll_mgmt_t *epoll, int port, struct http_events_t *events);

#endif
