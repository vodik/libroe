#include "test_http.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <util.h>
#include <hashtable.h>

static void
echo_message(ws_t *ws, const char *msg, size_t len)
{
	ws_send(ws, msg, len);
}

////////////////////////////////////////////////////////////////////////////////

void
test_onrequest(http_t *conn, request_t *request, response_t *response)
{
	printf("*** we are here requesting!\n");
	if (strcmp(request->method, "GET") != 0) {
		fprintf(stderr, "only GET supported at the moment\n");
		return;
	}

	FILE *file = fopen("log", "a+");
	char *useragent = NULL;

	fprintf(file, ":: logging request\n   > method:\t%s\n   > path:\t%s\n", request->method, request->path);
	if ((useragent = hashtable_get(&request->headers, "User-Agent")))
		fprintf(file, ":: user-agent: %s\n", useragent);
	fputs("\n", file);
	fclose(file);

	int fd, filesize;
	char *map;
	char buf[10];

	printf("--> sending file\n");
	fd = open(request->path + 1, O_RDONLY); /* paths start with / */
	if (fd == -1) {
		response_header_set(response, 404, NULL);
		return;
	}
	else
		response_header_set(response, 200, NULL);

	filesize = lseek(fd, 0, SEEK_END);
	map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
	itoa(filesize, buf, 10, 10);

	response_header_add(response, "Content-Type", "text/html");
	response_header_add(response, "Content-Length", buf);
	response_write(response, map, filesize);
	printf("--> sent\n");

	munmap(map, filesize);
	close(fd);

	//conn_close(&conn->base);
}

void
ws_onrequest(ws_t *ws)
{
	//if (sbuf_cmp(ws->path, "/echo") == 0) {
		/* TODO */
	//} else
		//ws_close(ws);
}
