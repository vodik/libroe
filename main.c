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

#include <response/response.h>

/* TODO: security. This can escape root with a malformed request (browsers filter /../ though) */
void send_file(http_response *response, const char *path, const char *mime)
{
	int fd, filesize;
	char *map;

	printf("--> sending file\n");
	fd = open(path + 1, O_RDONLY); /* paths start with / */
	if (fd == -1) {
		http_response_error(response, 404, "Not Found");
		return;
	}

	filesize = lseek(fd, 0, SEEK_END);
	map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);

	http_response_begin(response, TRANSFER_ENCODING_NONE, 200, "OK", mime, filesize);
	http_response_write(response, map, filesize);
	http_response_end(response);
	printf("--> sent\n");

	munmap(map, filesize);
	close(fd);
}

/** 
* @brief 
* 
* @param request
* @param response
* 
* @return 
*/
/*int http_get(http_conn *conn, const char *path, size_t len, http_response *response)
{
	if (strcmp(path, "/favico.ico")) {
		http_conn_set_header_reader(logger);
		http_conn_set_server(favico_server);
	}
}

int http_post(struct http_request_t *request, http_response *response)
{
	printf("--- got a post, handling it like a get for now\n");
	return http_get(request, response);
}

static struct http_events_t http_handler = {
	.cbs[HTTP_METHOD_GET]   = http_get,
	.cbs[HTTP_METHOD_POST]  = http_post,
};*/

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
