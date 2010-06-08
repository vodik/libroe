#include "response.h"

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

#include <sbuf.h>
#include <conn.h>

char *
def_message(int code)
{
	switch (code) {
		case 200:
			return strdup("OK");
		case 404:
			return strdup("Not Found");
	}
	return NULL;
}

void
response_write_header(response_t *response)
{
	sbuf_t header;
	static const char template[] =
		"HTTP/1.1 %d %s\r\n";

	sbuf_init(&header, 0);
	sbuf_scatf(&header, template, response->code, response->message);
	sbuf_cat(&header, "\r\n");

	conn_write(response->conn, sbuf_raw(&header), sbuf_len(&header));
	sbuf_cleanup(&header);
}

////////////////////////////////////////////////////////////////////////////////

void
response_init(response_t *response, conn_t *conn)
{
	response->conn = conn;
	response->wrote_headers = 0;
	hashtable_init(&response->headers, 16, NULL);
}

void
response_cleanup(response_t *response)
{
	if (response->message)
		free(response->message);
	hashtable_cleanup(&response->headers, free);
}

void
response_header_set(response_t *response, int code, const char *message)
{
	response->code = code;
	response->message = message == NULL ? def_message(code) : strdup(message);
}

void
response_header_add(response_t *response, const char *header, const char *field)
{
	hashtable_add(&response->headers, header, strdup(field));
}

void
response_write(response_t *response, const char *buf, size_t len)
{
	if (!response->wrote_headers) {
		response_write_header(response);
		response->wrote_headers = 1;
	}

	conn_write(response->conn, buf, len);
}
