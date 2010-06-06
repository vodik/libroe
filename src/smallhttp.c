#include "smallhttp.h"

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <http.h>
#include <websocket.h>

int
smallhttp_start(smallhttpd_t *httpd, int size, http_iface_t *iface)
{
	poll_mgmt_start(&httpd->polling, size);
	service_start(&httpd->services[0], "http", &httpd->polling, iface->port, iface);
	return 0;
}

int
smallhttp_open_websocket(smallhttpd_t *httpd, ws_iface_t *iface)
{
	service_start(&httpd->services[0], "websocket", &httpd->polling, iface->port, iface);
	return 0;
}

int
smallhttp_run(smallhttpd_t *httpd)
{
	int running = 0;
	while(running == 0) {
		printf("--> loop start\n");
		running = poll_mgmt_poll(&httpd->polling, -1);
		if (running == 0)
			printf("--> loop repeat - %d, %s\n", running, strerror(running));
	}
	return running;
}

void
smallhttp_stop(smallhttpd_t *httpd)
{
	poll_mgmt_stop(&httpd->polling);
}
