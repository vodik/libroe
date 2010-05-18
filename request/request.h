#ifndef SMALLHTTP_HTTP_REQUEST
#define SMALLHTTP_HTTP_REQUEST

#include <util/hashtable.h>

typedef struct {
	int method;
	char *path;
	char *args;
	int version_major, version_minor;

	hashtable_t headers;
} http_request;

void http_request_init(http_request *);
void http_request_free(http_request *);

#endif
