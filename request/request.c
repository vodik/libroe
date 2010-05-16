#include "request.h"

#include <stdlib.h>
#include <util/hashtable.h>

void http_request_init(http_request *request, int fd)
{
	request->fd = fd;
	hashtable_init(16, NULL, &request->headers);
}

void http_request_free(http_request *request)
{
	if (request->path)
		free(request->path);
	hashtable_free(&request->headers, 1);
}
