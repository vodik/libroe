#include "http_parser.h"

state_fn state_method, state_path, state_version, state_header, state_value;

int state_method(http_parser *parser)
{
	char *b = parser->buf;
	size_t nbytes = parser->nbytes;

	while (nbytes-- > 0 && *b != ' ')
		 ++b;

	
}

////////////////////////////////////////////////////////////////////////////////

void http_parser_init(http_parser *)
{
}

void http_parser_set_buffer(http_parser *parser, const char *buf, size_t nbytes)
{
	parser->buf = buf;
	parser->nbytes = nbytes;
}

struct http_event *http_parser_next_event(http_parser *)
{
	if (parser->nbytes < 0 && parser->next_state == NULL)
		return NULL;

	static http_event event;
	parser->next_state(parser, &event);
	return &event;
}
