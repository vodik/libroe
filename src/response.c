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
#define CM(code, msg) case code: return strdup(msg)
	switch (code) {
		CM(100, "Continue");
		CM(101, "Switching Protocols");
		
		CM(200, "OK");
		CM(201, "Created");
		CM(202, "Accepted");
		CM(203, "Non-Authoritative Information");
		CM(204, "No Content");
		CM(205, "Reset Content");
		CM(206, "Partial Content");

		CM(300, "Multiple Choices");
		CM(301, "Moved Permanently");
		CM(302, "Found");
		CM(303, "See Other");
		CM(304, "Not Modified");
		CM(305, "Use Proxy");
		CM(307, "Termporary Redirect");

		CM(400, "Bad Request");
		CM(401, "Unauthorized");
		CM(402, "Payment Required");
		CM(403, "Forbidden");
		CM(404, "Not Found");
		CM(405, "Method Not Allowed");
		CM(406, "Not Acceptable");
		CM(407, "Proxy Authentication Required");
		CM(408, "Request Timeout");
		CM(409, "Conflict");
		CM(410, "Gone");
		CM(411, "Length Required");
		CM(412, "Precondition Failed");
		CM(413, "Request Entry Too Large");
		CM(414, "Request-URI Too Large");
		CM(415, "Unsupported Media Type");
		CM(416, "Request Range Not Satisfiable");
		CM(417, "Expectation Failed");
		CM(418, "Conflict");

		CM(500, "Internal Server Error");
		CM(501, "Not Implemented");
		CM(502, "Bad Gateway");
		CM(503, "Service Unavailable");
		CM(504, "Gateway Timeout");
		CM(505, "HTTP Version Not Supported");
	}
	return NULL;
#undef CM
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
	if (!response->wrote_headers) {
		response_write_header(response);
		response->wrote_headers = 1;
	}

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
