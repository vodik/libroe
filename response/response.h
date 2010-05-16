#ifndef SMALLHTTP_RESPONSE
#define SMALLHTTP_RESPONSE

#include <request/request.h>

enum {
	WRITE_STREAM,
	WRITE_CHUNKED,
};

typedef struct {
	char *buf;
	int fd;

	hashashtable headers;
} http_response;

void http_repsonse_init(http_response *, int fd);
void http_response_free(http_response *);

void http_response_send(const http_request const *request, int code, const char *message, http_response *response);
void http_response_send_chunck(const http_request const *request, int code, const char *message, http_response *response);

#endif
