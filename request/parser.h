#ifndef SMALLHTTP_REQUEST_PARSER
#define SMALLHTTP_REQUEST_PARSER

#include <stddef.h>

struct event_t;
struct state_t;
//typedef int state_fn(struct state_t *);/*, const char **, int *);*/

enum http_methods {
	HTTP_DELETE = 0x00,
	HTTP_GET    = 0x01,
	HTTP_HEAD   = 0x02,
	HTTP_POST   = 0x03,
	HTTP_PUT    = 0x04,
};

enum http_parser_data {
	HTTP_DATA_METHOD,
	HTTP_DATA_PATH,
	HTTP_DATA_QUERY,
	HTTP_DATA_URL,
	HTTP_DATA_FRAGMENT,
	HTTP_DATA_VERSION,
	HTTP_DATA_HEADER,
	HTTP_DATA_FIELD,
};

enum http_parser_events {
	HTTP_EVENT_DONE,
	HTTP_EVENT_UPGRADE,
};

typedef struct {
	enum http_parser_data type;
	const char *buf;
	size_t nbytes;
	int done;
} http_parser_event;

/*struct http_parser_state {
	state_fn *next_state;
	struct http_event *event;
};*/

typedef struct {
	int type;
} event_data_t;

struct http_parser;
typedef int state_fn(struct state_t *state, char *buf, size_t nbytes, event_data_t *data);

struct state_t {
	char *buf;
	size_t len;
	state_fn *next;
};

typedef struct http_parser {
	struct state_t state;
	http_parser_event event;
	char *buf;
	size_t len;
} http_parser;

/*void http_parser_init(http_parser *);
void http_parser_set_buffer(http_parser *, char *buf, size_t nbytes);
struct http_event *http_parser_next_event(http_parser *);*/

#endif
