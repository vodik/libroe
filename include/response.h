#ifndef LIBROE_RESPONSE
#define LIBROE_RESPONSE

#include <stddef.h>
#include <conn.h>
#include <hashtable.h>

typedef struct {
	conn_t *conn;
	int wrote_headers;

	int code;
	char *message;
	hashtable_t headers;
} response_t;

void response_init(response_t *response, conn_t *conn);
void response_cleanup(response_t *response);

void response_header_set(response_t *response, int code, const char *message);
void response_header_add(response_t *response, const char *header, const char *field);
void response_write(response_t *response, const char *buf, size_t len);

#endif
