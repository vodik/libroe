#ifndef SMALLHTTP_HTTP_REQUEST
#define SMALLHTTP_HTTP_REQUEST

#include <util/hashtable.h>

typedef struct {
	int fd;
	int method;
	char *path;
	char *args;
	int version_major, version_minor;

	hashtable headers;
} http_request;

void http_request_init(http_request *, int fd);
void http_request_free(http_request *);

#endif
