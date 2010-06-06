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

int
http_pull_request(http_iface_t *iface, http_t *conn, http_parser *reader)
{
	int code;
	const char *b;
	size_t len;

	while ((code = http_parser_next(reader, &b, &len)) > 0) {
		switch (code) {
			case HTTP_DATA_METHOD:
				printf("==> METHOD: \"%s\"\n", b);
				conn->method = strndup(b, len);
				break;
			case HTTP_DATA_PATH:
				printf("==> PATH: \"%s\"\n", b);
				conn->path = strndup(b, len);
				break;
			case HTTP_DATA_VERSION:
				printf("==> VERSION: \"%s\"\n", b);
				conn->version = strndup(b, len);

				/* handle request */
				assert(iface);
				if (iface && iface->onrequest)
					iface->onrequest(conn);

				return HTTP_EVT_DONE;
		}
	}
	return code;
}

int
http_handle_headers(http_t *conn, http_parser *reader)
{
	int code;
	const char *b;
	size_t len;

	static char *header = NULL;

	while ((code = http_parser_next(reader, &b, &len)) > 0) {
		switch (code) {
			case HTTP_DATA_HEADER:
				printf("==> HEADER: \"%s\"\n", b);
				header = strndup(b, len);
				break;
			case HTTP_DATA_FIELD:
				printf("==> FIELD: \"%s\"\n", b);

				/* handle headers */
				if (conn && conn->onheader)
					conn->onheader(conn, header, b);

				free(header);
				break;
		}
	}
	return code;
}

////////////////////////////////////////////////////////////////////////////////

void
http_on_open(conn_t *conn)
{
	printf("==> HTTP START: %d\n", conn->fd);
}

int
http_on_message(conn_t *conn, void *data)
{
	printf("==> HTTP MESSAGE: %d\n", conn->fd);
	http_t *httpc = (http_t *)conn;

	http_parser reader;
	int code;

	http_parser_init(&reader, conn, 512, 2);
	code = http_pull_request(data, httpc, &reader);
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			printf("==> TIMEOUT\n");
			return 0;
		case HTTP_EVT_ERROR:
			die("error");
			break;
	}

	/* send request */
	printf("==> QUERY REQUEST!\n");

	/* send headers */
	code = http_handle_headers(httpc, &reader);
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			printf("==> TIMEOUT\n");
			return 0;
		case HTTP_EVT_ERROR:
			die("error");
			break;
	}
	http_parser_cleanup(&reader);

	/* on body */
	http_response response;
	http_response_init(&response, conn);

	/* send */

	http_response_cleanup(&response);

	return 0;
}

void
http_on_close(conn_t *conn)
{
	printf("==> HTTP CLOSE: %d\n", conn->fd);
}

void
http_destroy(void *conn)
{
	printf("==> HTTP CLEANUP\n");
	http_t *hconn = (http_t *)conn;
	free(hconn->method);
	free(hconn->path);
	free(hconn->version);
}
