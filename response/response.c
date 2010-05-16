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

void response_send(const http_request const *request, int error, const char *message, response_writer *response)
{
	char buffer[BUFSIZ];

	int fd = request->fd;
	int len = sprintf(buffer, "HTTP/1.1 %i %s\r\nContent-Type: %s\r\n\r\n", error, message, "text/plain");
	printf("%d: %s\n", len, buffer);
	int r = send(fd, buffer, len, 0);
	printf("got %d\n", r);
}
