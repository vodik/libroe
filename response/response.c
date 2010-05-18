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
#include <util/hashtable.h>

void http_response_init(http_response *response, int fd)
{
	response->sending = 0;
	response->fd = fd;
	response->encoding = TRANSFER_ENCODING_NONE;
}

void http_response_begin(http_response *response, int encoding, int code, const char *msg, const char *mime, int content_length)
{
	static char buffer[BUFSIZ];
	int len, fd = response->fd;

	response->encoding = encoding;
	response->sending = 1;

	switch (encoding) {
		case TRANSFER_ENCODING_NONE:
			len = sprintf(buffer, "HTTP/1.1 %i %s\r\nContent-Type: %s\r\nContent-Length: %i\r\n\r\n", code, msg, mime, content_length);
			break;
		case TRANSFER_ENCODING_CHUNKED:
			len = sprintf(buffer, "HTTP/1.1 %i %s\r\nContent-Type: %s\r\nTransfer-Encoding: %s\r\n\r\n", code, msg, mime, "chunked");
			break;
	}
	send(fd, buffer, len, 0);
}

void http_response_error(http_response *response, int code, const char *msg)
{
	static char buffer[BUFSIZ];
	int len, fd = response->fd;

	response->sending = -1;
	len = sprintf(buffer, "HTTP/1.1 %i %s\r\n\r\n", code, msg);
	send(fd, buffer, len, 0);
}

void http_response_write(http_response *response, const char *data, size_t nbytes)
{
	static char buffer[10];

	if (!response->sending)
		return;

	int len, fd = response->fd;
	switch (response->encoding) {
		case TRANSFER_ENCODING_NONE:
			send(fd, data, nbytes, 0);
			break;
		case TRANSFER_ENCODING_CHUNKED:
			len = sprintf(buffer, "%x\r\n", nbytes);
			send(fd, buffer, len, 0);
			send(fd, data, nbytes, 0);
			send(fd, "\r\n\r\n", 4, 0);
			break;
	}
}

void http_response_end(http_response *response)
{
	int fd = response->fd;

	response->sending = 0;

	if (response->encoding == TRANSFER_ENCODING_CHUNKED) {
		send(fd, "0\r\n\r\n", 5, 0);
	}
}

