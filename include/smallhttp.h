#ifndef SMALLHTTP
#define SMALLHTTP

#include <poll_mgmt.h>
#include <services.h>
#include <http.h>
#include <websocket.h>

typedef struct {
	poll_mgmt_t polling;
	service_t services[2]; /* TODO: linked list so allow dynamic growth */
} smallhttpd_t;

int smallhttp_start(smallhttpd_t *httpd, int size, http_iface_t *iface);
int smallhttp_open_websocket(smallhttpd_t *httpd, ws_iface_t *iface);
int smallhttp_run(smallhttpd_t *httpd);
void smallhttp_stop(smallhttpd_t *httpd);

#endif
