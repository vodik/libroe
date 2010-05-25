#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include "services.h"
#include <response/response.h>
#include <request/parser.h>

typedef int (*request_cb)(const char *msg, size_t nbytes, event_data_t *evt, http_response *response);

struct http_events_t {
	request_cb cbs[LAST_HTTP_METHOD];
};

void http_start(struct service_t *http, poll_mgmt_t *mgmt, int port, struct http_events_t *events);

#endif
