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
	.port = 33456,
	.onrequest = test_onrequest,
};

int main(int argc, char *argv[])
{
	/*poll_mgmt_t mgmt;
	struct service_t services[2];*/

	smallhttpd_t httpd;
	int http_port     = 44567;
	int websocks_port = 33456;
	char c;

	while ((c = getopt(argc, argv, "p:w:")) != -1)
	{
		switch (c) {
			case 'p':
				http_port = atoi(optarg);
				break;
			case 'w':
				websocks_port = atoi(optarg);
				break;
		}
	}

	test_ops.port = http_port;

	smallhttp_start(&httpd, POLL_EVENTS, &test_ops);
	smallhttp_run(&httpd);
	smallhttp_stop(&httpd);

	/*poll_mgmt_start(&mgmt, POLL_EVENTS);
	http_start(&services[0], &mgmt, http_port, &http_handler);
	websocks_start(&services[1], &mgmt, websocks_port, NULL);

	printf("http://localhost:%d/post.html\n", http_port);

	int running = 0;
	while(running == 0) {
		printf("--> loop start\n");
		running = poll_mgmt_poll(&mgmt, -1);
		if (running == 0)
			printf("--> loop repeat - %d, %s\n", running, strerror(running));
	}
	
	printf("==> ending - %d, %s\n", running, strerror(running));
	poll_mgmt_stop(&mgmt);*/
}
