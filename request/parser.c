#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

state_fn state_method, state_path, state_version, state_header, state_field;

int state_method(struct state_t *state, char *buf, size_t nbytes, event_data_t *evt)
{
	int read = 0;

	printf("&&& method\n");
	while (state->len-- > 0 && (*buf = *state->buf++) != ' ' && read++ < nbytes)
		++buf;

	evt->type = HTTP_DATA_METHOD;
	state->next = state_path;

	return read;
}

int state_path(struct state_t *state, char *buf, size_t nbytes, event_data_t *evt)
{
	int read = 0;

	while (state->len-- > 0 && (*buf = *state->buf++) != ' ' && read++ < nbytes)
		++buf;

	evt->type = HTTP_DATA_PATH;
	state->next = state_version;

	return read;
}

int state_version(struct state_t *state, char *buf, size_t nbytes, event_data_t *evt)
{
	int read = 0;

	while (state->len-- > 0 && (*buf = *state->buf++) != '\r' && read++ < nbytes)
		++buf;
	++state->buf;
	--state->len;

	evt->type = HTTP_DATA_VERSION;
	state->next = state_header;

	return read;
}

int state_header(struct state_t *state, char *buf, size_t nbytes, event_data_t *evt)
{
	int read = 0;

	assert(state->len < 2000);
	if (state->len == 2) {
		state->len = 0;
		state->next = NULL;
		return 0;
	}

	while (state->len-- > 0 && (*buf = *state->buf++) != ':' && read++ < nbytes)
		++buf;
	++state->buf;
	--state->len;

	evt->type = HTTP_DATA_HEADER;
	state->next = state_field;

	return read;
}

int state_field(struct state_t *state, char *buf, size_t nbytes, event_data_t *evt)
{
	int read = 0;

	assert(state->len < 2000);

	while (state->len-- > 0 && (*buf = *state->buf++) != '\r' && read++ < nbytes)
		++buf;
	++state->buf;
	--state->len;

	evt->type = HTTP_DATA_FIELD;
	state->next = state_header;

	return read;
}

////////////////////////////////////////////////////////////////////////////////

void http_parser_init(http_parser *parser)
{
	//parser->buffer = malloc(bufsize);
	parser->state.next = state_method;
	parser->state.len = 0;
}

void http_parser_set_buffer(http_parser *parser, const char *buf, size_t nbytes)
{
	parser->state.buf = buf;
	parser->state.len = nbytes;
}

int http_parser_next_event(http_parser *parser, char *buf, size_t nbytes, event_data_t *evt)
{
	//http_parser_event *event = &parser->event;
	int event = -1;

	if (parser->state.len > 0 && parser->state.next)
		event = parser->state.next(&parser->state, buf, nbytes, evt);

	return event;
}
