#ifndef SMALLHTTP
#define SMALLHTTP

#include <poll_mgmt.h>
#include <services/services.h>

typedef struct {
	poll_mgmt_t polling;
	struct service_t services[2]; /* TODO: linked list so allow dynamic growth */
} smallhttpd_t;

int smallhttp_start(smallhttpd_t *httpd, int size, int port);
int smallhttp_open_websocket(smallhttpd_t *httpd, int port);
void smallhttp_stop(smallhttpd_t *httpd);

#endif
