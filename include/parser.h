#ifndef SMALLHTTP_REQUEST_PARSER
#define SMALLHTTP_REQUEST_PARSER

#include <stddef.h>
#include <sys/time.h>
#include <conn.h>
#include <ibuf.h>
#include <sbuf.h>

enum http_methods {
	HTTP_METHOD_DELETE,
	HTTP_METHOD_GET,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,

	HTTP_METHOD_CONNECT,
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_TRACE,

#ifdef WEBDAV
	HTTP_METHOD_COPY,
	HTTP_METHOD_LOCK,
	HTTP_METHOD_MKCOL,
	HTTP_METHOD_MOVE,
	HTTP_METHOD_PROPFIND,
	HTTP_METHOD_PROPPATCH,
	HTTP_METHOD_UNLOCK,
#endif

	LAST_HTTP_METHOD,
};

enum http_parser_evt {
	HTTP_EVT_DONE,

	HTTP_DATA_METHOD,
	HTTP_DATA_PATH,
	HTTP_DATA_VERSION,
	HTTP_DATA_HEADER,
	HTTP_DATA_FIELD,
	LAST_HTTP_DATA,

	HTTP_EVT_ERROR,
	HTTP_EVT_TIMEOUT,
};

struct state_t;
typedef int state_fn(struct state_t *state);

struct ibuf_store {
	int fd;
	struct timeval tv;
};

struct state_t {
	ibuf_t buf;
	sbuf_t dest;
	int state;
};

typedef struct http_parser {
	struct state_t state;
	struct ibuf_store store;
} http_parser;

void http_parser_init(http_parser *parser, conn_t *conn, size_t size, int timeout);
void http_parser_cleanup(http_parser *parser);
int http_parser_next(http_parser *parser, const char **buf, size_t *len);

#endif
