#include "test_http.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

static void
static_serve(http_t *conn)
{
	int fd, filesize;
	char *map;

	printf("--> sending file\n");
	fd = open(conn->path + 1, O_RDONLY); /* paths start with / */
	if (fd == -1) {
		http_response_error(&conn->response, 404, "Not Found");
		return;
	}

	filesize = lseek(fd, 0, SEEK_END);
	map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);

	http_response_begin(&conn->response, TRANSFER_ENCODING_NONE, 200, "OK", "text/html", filesize);
	http_response_write(&conn->response, map, filesize);
	http_response_end(&conn->response);
	printf("--> sent\n");

	munmap(map, filesize);
	close(fd);

	//conn->keep_alive = 0;
}

static void
echo_message(ws_t *ws, const char *msg, size_t len)
{
	ws_send(ws, msg, len);
}

static void
logger(http_t *conn, const char *header, const char *field)
{
	printf("*** we are here logging!\n");
	if (strcmp(header, "User-Agent") == 0) {
		FILE *file = fopen("useragents", "a+");
		fprintf(file, "=== user-agent: %s\n", field);
		fclose(file);
	}
}

////////////////////////////////////////////////////////////////////////////////

void
test_onrequest(http_t *conn)
{
	printf("*** we are here requesting!\n");
	if (strcmp(conn->method, "GET") != 0) {
		fprintf(stderr, "only GET supported at the moment\n");
		conn->keep_alive = 0;
	}

	printf(":: logging request\n   > method:\t%s\n   > path:\t%s\n", conn->method, conn->path);

	conn->onheader = logger;
	conn->makeresponse = static_serve;
}

void
ws_onrequest(ws_t *ws)
{
	//if (sbuf_cmp(ws->path, "/echo") == 0) {
		/* TODO */
	//} else
		//ws_close(ws);
}
