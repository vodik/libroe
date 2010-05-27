#ifndef SMALLHTTP_REQUEST_PARSER
#define SMALLHTTP_REQUEST_PARSER

#include <stddef.h>

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

enum http_parser_data {
	HTTP_DATA_METHOD,
	HTTP_DATA_PATH,
	/*HTTP_DATA_QUERY,
	HTTP_DATA_URL,
	HTTP_DATA_FRAGMENT,*/
	HTTP_DATA_VERSION,
	HTTP_DATA_HEADER,
	HTTP_DATA_FIELD,
};

typedef struct {
	int type;
} event_data_t;

struct state_t;
typedef int state_fn(struct state_t *state, char *buf, size_t nbytes, event_data_t *data);

struct state_t {
	const char *buf;
	size_t len;
	state_fn *next;
};

typedef struct http_parser {
	struct state_t state;
} http_parser;

void http_parser_init(http_parser *);
void http_parser_set_buffer(http_parser *, const char *buf, size_t nbytes);
int http_parser_next_event(http_parser *parser, char *buf, size_t nbytes, event_data_t *evt);

#endif
