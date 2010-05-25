#ifndef SMALLHTTP_REQUEST_PARSER
#define SMALLHTTP_REQUEST_PARSER

#include <stddef.h>

struct event_t;
struct state_t;
//typedef int state_fn(struct state_t *);/*, const char **, int *);*/

enum request_methods {
	HTTP_DELETE = 0x00,
	HTTP_GET    = 0x01,
	HTTP_HEAD   = 0x02,
	HTTP_POST   = 0x03,
	HTTP_PUT    = 0x04,
};

enum request_parser_data {
	HTTP_DATA_METHOD,
	HTTP_DATA_PATH,
	HTTP_DATA_QUERY,
	HTTP_DATA_URL,
	HTTP_DATA_FRAGMENT,
	HTTP_DATA_VERSION,
	HTTP_DATA_HEADER,
	HTTP_DATA_FIELD,
};

enum request_parser_events {
	HTTP_EVENT_DONE,
	HTTP_EVENT_UPGRADE,
};

typedef struct {
	enum request_parser_data type;
	const char *buf;
	size_t nbytes;
	int done;
} request_parser_event;

/*struct request_parser_state {
	state_fn *next_state;
	struct request_event *event;
};*/

typedef struct {
	int type;
} event_data_t;

struct request_parser;
typedef int state_fn(struct state_t *state, char *buf, size_t nbytes, event_data_t *data);

struct state_t {
	char *buf;
	size_t len;
	state_fn *next;
};

typedef struct request_parser {
	struct state_t state;
	request_parser_event event;
} request_parser;

void request_parser_init(request_parser *);
void request_parser_set_buffer(request_parser *, char *buf, size_t nbytes);
int request_parser_next_event(request_parser *parser, char *buf, size_t nbytes, event_data_t *evt);

#endif
