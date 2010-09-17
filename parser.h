#ifndef LIBROE_REQUEST_PARSER
#define LIBROE_REQUEST_PARSER

#include <stddef.h>
#include <sys/time.h>
#include "io.h"

enum methods {
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

enum parser_evt {
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

struct parser;

struct parser *parser_new(IO *io);
void parser_cleanup(struct parser *parser);
int parser_next(struct parser *parser, const char **buf, size_t *len);

#endif
