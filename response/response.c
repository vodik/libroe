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

/*void response_send(const http_request const *request, int code, const char *message, http_response *response)
{
	char buffer[BUFSIZ];

	int fd = response->fd;
	int len = sprintf(buffer, "HTTP/1.1 %i %s\r\nContent-Type: %s\r\n\r\n", code, message, "text/plain");
	printf("%d: %s\n", len, buffer);
	int r = send(fd, buffer, len, 0);
	printf("got %d\n", r);
}*/

void http_response_init(http_response *response, int fd)
{
	response->fd = fd;
	hashtable_init(16, NULL, &response->headers);
}

/* FIXME: clean up all hashtable entries */
void http_response_free(http_response *response)
{
	hashtable_free(&response->headers, 1);
}
