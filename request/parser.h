#ifndef SMALLHTTP_HTTP_PARSER
#define SMALLHTTP_HTTP_PARSER

#include <request/request.h>

#define BUFFER_LENGTH 1024

enum http_requests {
	HTTP_HEAD,
	HTTP_GET,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_TRACE,
	HTTP_OPTIONS,
	HTTP_CONNECT,
	HTTP_PATCH,
};

struct state;

typedef int state_fn(struct state *, const char **, int *);
typedef int parse_fn(struct state *);

struct state {
	state_fn *next;
	parse_fn *parse;
	void *arg;
	char buf[BUFFER_LENGTH];
	char *tmp;
	int len;
	int done;
};

typedef struct {
	http_request request;
	struct state state;
} http_parser;

void http_parser_init(http_parser *, int fd);
void http_parser_free(http_parser *);
int http_parser_read(http_parser *, const char *buf, int len);
const http_request *http_parser_done(http_parser *);

#endif
