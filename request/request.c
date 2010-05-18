#include "request.h"

#include <stdlib.h>
#include <util/hashtable.h>

void http_request_init(http_request *request)
{
	hashtable_init(&request->headers, 16, NULL);
}

/* FIXME: clean up all hashtable entries */
void http_request_free(http_request *request)
{
	if (request->path)
		free(request->path);
	if (request->args)
		free(request->args);
	hashtable_cleanup(&request->headers, free);
}
