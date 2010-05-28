#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

#include <util.h>
#include <util/urlencode.h>
#include <smallhttp.h>
#include "config.h"

#include "test_http.h"

#include <response/response.h>

struct http_ops test_ops = {
	.port = 11234,
	.onrequest = test_onrequest,
};

struct ws_ops ws_ops = {
	.port = 33456,
	//.onmessage = NULL,//test_onmessage,
};

int
main(int argc, char *argv[])
{
	/*poll_mgmt_t mgmt;
	struct service_t services[2];*/

	smallhttpd_t httpd;
	char c;

	while ((c = getopt(argc, argv, "p:w:")) != -1)
	{
		switch (c) {
			case 'p':
				test_ops.port = atoi(optarg);
				break;
			case 'w':
				ws_ops.port = atoi(optarg);
				break;
		}
	}

	smallhttp_start(&httpd, POLL_EVENTS, &test_ops);
	smallhttp_open_websocket(&httpd, &ws_ops);

	smallhttp_run(&httpd);
	smallhttp_stop(&httpd);
}
