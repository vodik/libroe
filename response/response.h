#ifndef SMALLHTTP_RESPONSE
#define SMALLHTTP_RESPONSE

#include <request/request.h>

typedef struct {
	char *buf;
} response_writer;

void response_send(const http_request const *request, int error, const char *message, response_writer *response);

#endif
