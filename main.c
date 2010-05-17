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
#include <socks.h>
#include <services/http.h>
#include <services/websocks.h>

/* FIXME: this is a quick and dirty hackish implementation */
void parse_args(hashtable *table, const char *args)
{
	char arg[1024];
	char val[1024];
	char dec[1024 * 3];
	char *mode = arg;
	int len = 0;

	while(*args) {
		if (*args == '=') {
			mode = val;
			len = 0;
		} else if (*args == '&') {
			*mode = '\0';
			len = url_decode(dec, val);
			//hashtable_insert(table, arg, strndup(dec, len));
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
	//hashtable_insert(table, arg, strndup(dec, len));
}

/* TODO: security. This can escape root with a malformed request (browsers filter /../ though) */
void send_file(http_response *response, const char *path, const char *mime)
{
	int fd, filesize;
	char *map;

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

	munmap(map, filesize);
	close(fd);
}

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
		/*hashtable table;
		hashtable_init(16, NULL, &table);
		parse_args(&table, request->args);

		printf("==> Message 1: \"%s\"\n",   (char *)hashtable_get(&table, "msg1"));
		printf("==> Message 2: \"%s\"\n\n", (char *)hashtable_get(&table, "msg2"));*/
	}

	if (strcmp(request->path, "/favicon.ico") == 0)
		send_file(response, request->path, "image/vnd.microsoft.icon");
	else
		send_file(response, request->path, "text/html");
	return 1;
}

static struct http_events_t http_handler = {
	.GET  = http_get,
	.POST = http_get,
	.PUT  = NULL,
};

int running = 0;
struct epoll_t epoll;
struct service_t *services[2];

static void sigterm()
{
	fprintf(stderr, "HANDLED term!\n");
	running = 1;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sigterm);
	signal(SIGTERM, sigterm);

	epoll_init(&epoll, 10);

	services[0] = http_start(&epoll, PORT1, &http_handler);
	//services[1] = websocks_start(&epoll, PORT2, NULL);

	printf("http://localhost:%d/index.html\n", PORT1);

	while(running == 0)
		running = epoll_poll(&epoll, -1);
	
	printf("==> ending - %d, %s", running, strerror(running));
	service_end(&epoll, services[0]);
	epoll_stop(&epoll);
}
