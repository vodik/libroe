#include "request.h"

#include <stdlib.h>
#include <util/hashtable.h>

void http_request_init(http_request *request)
{
	hashtable_init(16, NULL, &request->headers);
}
