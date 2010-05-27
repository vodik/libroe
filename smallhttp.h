#ifndef SMALLHTTP
#define SMALLHTTP

#include <poll_mgmt.h>
#include <services/services.h>
#include <services/http.h>

typedef struct {
	poll_mgmt_t polling;
	struct service_t services[2]; /* TODO: linked list so allow dynamic growth */
} smallhttpd_t;

int smallhttp_start(smallhttpd_t *httpd, int size, struct http_ops *ops);
int smallhttp_open_websocket(smallhttpd_t *httpd, int port);
int smallhttp_run(smallhttpd_t *httpd);
void smallhttp_stop(smallhttpd_t *httpd);

#endif
