#ifndef SMALLHTTP_RESPONSE
#define SMALLHTTP_RESPONSE

#include <stddef.h>
#include <conn.h>
#include <obuf.h>

enum {
	TRANSFER_ENCODING_NONE = 0,
	TRANSFER_ENCODING_CHUNKED
};

typedef struct {
	/*obuf_t buf;*/
	int reserve;
} http_response;

void http_response_init(http_response *response, conn_t *conn, int code);
void http_response_cleanup(http_response *response);

void http_response_set_header(http_response *response, const char *header, const char *field);
void http_response_write(http_response *response, const char *buf, size_t len);

#endif
