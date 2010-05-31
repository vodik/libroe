#ifndef SMALLHTTP
#define SMALLHTTP

#include <poll_mgmt.h>
#include <services.h>
#include <http.h>
#include <websocks.h>

typedef struct {
	poll_mgmt_t polling;
	struct service_t services[2]; /* TODO: linked list so allow dynamic growth */
} smallhttpd_t;

int smallhttp_start(smallhttpd_t *httpd, int size, struct http_iface *iface);
int smallhttp_open_websocket(smallhttpd_t *httpd, struct ws_iface *iface);
int smallhttp_run(smallhttpd_t *httpd);
void smallhttp_stop(smallhttpd_t *httpd);

#endif
