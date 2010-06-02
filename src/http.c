#include "http.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include <parser.h>
#include <util.h>

void
http_on_open(conn_t *conn)
{
	printf("==> HTTP START: %d\n", conn->fd);
}

int
http_on_message(conn_t *conn)
{
	printf("==> HTTP MESSAGE: %d\n", conn->fd);

	http_parser reader;
	const char *buf;
	size_t len;
	int code;

	http_parser_init(&reader, conn, 60);
	while ((code = http_parser_next(&reader, &buf, &len)) > 0) {
		/* use buf, len */
	}
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			return 0;
		default:
			die("error");
	}
	http_parser_cleanup(&reader);

	/* send request */

	/* send headers */

	/* on body */
	http_response response;

	http_response_init(&response, conn);
	/* send */
	http_response_cleanup(&response);

	/* determine if keep alive */
	//return CONN_CLOSE;
	return 0;
}

void
http_on_close(conn_t *conn)
{
	printf("==> HTTP CLOSE: %d\n", conn->fd);
}
