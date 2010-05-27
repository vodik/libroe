#include "smallhttp.h"

#include "config.h"

#include <services/http.h>
#include <services/websocks.h>

int smallhttp_start(smallhttpd_t *httpd, int size, int port)
{
	poll_mgmt_start(&httpd->polling, size);
	http_start(&httpd->services[0], &httpd->polling, port, &http_handler);
	return 0;
}

int smallhttp_open_websocket(smallhttd_t *httpd, int port)
{
	websocks_start(&http->services[1], &httpd->polling, port, &websocks_handler);
	return 0;
}

int smallhttp_run(smallhttpd_t *httpd)
{
	int running = 0;
	while(running == 0) {
		printf("--> loop start\n");
		running = poll_mgmt_poll(&httpd->mgmt, -1);
		if (running == 0)
			printf("--> loop repeat - %d, %s\n", running, strerror(running));
	}
}

void smallhttp_stop(smallhttpd_t *httpd)
{
	poll_mgmt_stop(&httpd->polling);
}
