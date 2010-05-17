#define PORT1 22345
#define PORT2 33456

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <util.h>
#include <socks.h>
#include <services/http.h>
#include <services/websocks.h>

const char *page =
	"<html>\n"
	"  <head>\n"
	"    <style>\n"
	"      body {\n"
	"        font-family: trebuchet ms;\n"
	"        background: #424242; \n"
	"        color: #fff;\n"
	"        text-align: center;\n"
	"        margin-top:10em;\n"
	"      }\n"
	"    </style>\n"
	"  </head>\n"
	"  <body>\n"
	"    <h1>Error 404!</h1>\n"
	"    Page not found!\n"
	"  </body>\n"
	"</html>\n";

/* TODO: security. This can escape root with a malformed request (browsers filter /../ though) */
void send_file(http_response *response, const char *path, const char *mime)
{
	int fd, filesize;
	char *map;

	fd = open(path + 1, O_RDONLY);
	if (fd == -1) {
		http_response_begin(response, TRANSFER_ENCODING_NONE, 404, "Not Found", "text/html", strlen(page));
		http_response_write(response, page, strlen(page));
		http_response_end(response);
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
	printf(" > version:    HTTP/%i.%i\n", request->version_major, request->version_minor);
	printf(" > host:       %s\n", (char *)hashtable_get(&request->headers, "Host"));
	printf(" > user-agent: %s\n", (char *)hashtable_get(&request->headers, "User-Agent"));
	printf("\n");

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

int main(int argc, char *argv[])
{
	struct epoll_t epoll;
	epoll_init(&epoll, 10);

	struct service_t *services[2];
	services[0] = http_start(&epoll, PORT1, &http_handler);
	//services[1] = websocks_start(&epoll, PORT2, NULL);

	for (;;)
		epoll_poll(&epoll, -1);
}