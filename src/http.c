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
#include <response.h>
#include <util.h>

void
http_on_open(conn_t *conn)
{
	printf("==> HTTP START: %d\n", conn->fd);
}

int
http_on_message(conn_t *conn, const void *data)
{
	printf("==> HTTP MESSAGE: %d\n", conn->fd);

	http_t *httpc = (http_t *)conn;
	http_iface_t *iface = (http_iface_t *)data;

	parser_t parser;
	int code;

	hashtable_init(&httpc->request.headers, 16, NULL);

	parser_init(&parser, conn, 512, 2);
	code = pull_request(&httpc->request, &parser);
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			printf("==> TIMEOUT\n");
			return 0;
		case HTTP_EVT_ERROR:
			die("error");
			break;
	}

	/* handle headers here
	 * char *vale = hashtable_get("Connection");
	 * if (strcmp(value, "Close"))
	 * 		..
	 */

	response_init(&httpc->response, conn);
	printf("==> QUERY REQUEST!\n");
	if (iface && iface->onrequest)
		iface->onrequest(httpc, &httpc->request, &httpc->response);

	response_cleanup(&httpc->response);
	parser_cleanup(&parser);
	return 0;
}

void
http_on_close(conn_t *conn)
{
	printf("==> HTTP CLOSE: %d\n", conn->fd);
}

void
http_destroy(conn_t *conn)
{
	printf("==> HTTP CLEANUP\n");
	http_t *hconn = (http_t *)conn;

	free(hconn->request.method);
	free(hconn->request.path);
	free(hconn->request.version);
	hashtable_cleanup(&hconn->request.headers, free);
}

size_t
http_write(conn_t *conn, const char *msg, size_t nbytes)
{
	http_t *hconn = (http_t *)conn;

	response_write(&hconn->response, msg, nbytes);
	return 0;
}
