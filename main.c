#define PORT1 44567
#define PORT2 33456

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
#include <poll_mgmt.h>
#include <services/http.h>
#include <services/websocks.h>

/* FIXME: this is a quick and dirty hackish implementation */
void parse_args(hashtable_t *table, const char *args)
{
	char arg[512];
	char val[512];
	char dec[512 * 3];
	char *mode = arg;
	int len = 0;

	while(*args) {
		if (*args == '=') {
			mode = val;
			len = 0;
		} else if (*args == '&') {
			*mode = '\0';
			len = url_decode(dec, val);
			hashtable_add(table, arg, strndup(dec, len));
			mode = arg;
			len = 0;
		} else {
			*mode++ = *args;
			++len;
		}
		++args;
	}
	*mode = '\0';
	len = url_decode(dec, val);
	hashtable_add(table, arg, strndup(dec, len));
}

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
int http_get(const http_request const *request, http_response *response)
{
	printf("HTTP header:\n");
	printf(" > method:     %i\n", request->method);
	printf(" > path:       %s\n", request->path);
	printf(" > args:       %s\n", request->args);
	printf(" > version:    HTTP/%i.%i\n", request->version_major, request->version_minor);
	printf(" > host:       %s\n", (char *)hashtable_get(&request->headers, "Host"));
	printf(" > user-agent: %s\n", (char *)hashtable_get(&request->headers, "User-Agent"));
	printf("\n");

	if (request->args) {
		hashtable_t table;
		hashtable_init(&table, 16, NULL);
		parse_args(&table, request->args);

		printf("==> Message 1: \"%s\"\n",   (char *)hashtable_get(&table, "msg1"));
		printf("==> Message 2: \"%s\"\n\n", (char *)hashtable_get(&table, "msg2"));
		hashtable_cleanup(&table, free);
	}

	if (strcmp(request->path, "/favicon.ico") == 0)
		send_file(response, request->path, "image/vnd.microsoft.icon");
	else
		send_file(response, request->path, "text/html");
	return 0;
}

static struct http_events_t http_handler = {
	.GET  = http_get,
	.POST = http_get,
	.PUT  = NULL,
};

int main(int argc, char *argv[])
{
	poll_mgmt_t mgmt;
	struct service_t services[2];

	poll_mgmt_start(&mgmt, 10);

	http_start(&services[0], &mgmt, PORT1, &http_handler);
	websocks_start(&services[1], &mgmt, PORT2, NULL);

	printf("http://localhost:%d/ws.html\n", PORT1);

	int running = 0;
	while(running == 0) {
		printf("--> loop start\n");
		running = poll_mgmt_poll(&mgmt, -1);
		if (running == 0)
			printf("--> loop repeat - %d, %s\n", running, strerror(running));
	}
	
	printf("==> ending - %d, %s\n", running, strerror(running));
	//service_end(&mgmt, services[0]);
	poll_mgmt_stop(&mgmt);
}
