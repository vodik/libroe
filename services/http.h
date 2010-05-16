#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include "services.h"
#include <request/request.h>
#include <response/response.h>

typedef int (*request_cb)(const http_request const *request, response_writer *response);

struct http_events_t {
	request_cb GET;
	request_cb POST;
};

struct service_t *http_start(struct epoll_t *epoll, int port, struct http_events_t *events);

#endif
