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

void http_response_init(http_response *response, conn_t *conn, int code)
{
}

void http_response_cleanup(http_response *response)
{
}

void http_response_set_header(http_response *response, const char *header, const char *field)
{
}

void http_response_write(http_response *response, const char *buf, size_t len)
{
}
