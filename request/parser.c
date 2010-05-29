#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

state_fn state_method, state_path, state_version, state_header, state_field;

enum {
	STATE_CONTINUE,
	STATE_ACCEPT,
	STATE_FAIL,
};

int
state_method(struct state_t *state, char *buf, size_t nbytes)
{
	char c;

	while (state->len-- > 0 && state->read++ < nbytes) {
		c = *state->buf++;
		if (c == ' ') {
			return STATE_ACCEPT;
		} else if (c >= 'A' && c <= 'Z') {
			*buf++ = c;
		} else {
			return STATE_FAIL;
		}
	}
	return STATE_CONTINUE;
}

int
state_path(struct state_t *state, char *buf, size_t nbytes)
{
	char c;

	while (state->len-- > 0 && state->read++ < nbytes) {
		c = *state->buf++;
		switch (c) {
			case ' ':
				return STATE_ACCEPT;
			default:
				*buf++ = c;
				break;
		}
	}
	return STATE_CONTINUE;
}

int
state_version(struct state_t *state, char *buf, size_t nbytes)
{
	int term = 0;
	char c;

	while (state->len-- > 0 && state->read++ < nbytes) {
		c = *state->buf++;
		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
			default:
				*buf++ = c;
				term = 0;
				break;
		}
	}
	return STATE_CONTINUE;
}

int
state_header(struct state_t *state, char *buf, size_t nbytes)
{
	char c;

	/* DON'T CHECK IF PARSING IS DONE HERE, THATS NOT APPROPRIATE */

	while (state->len-- > 0 && state->read++ < nbytes) {
		c = *state->buf++;
		switch (c) {
			case ':':
				return STATE_ACCEPT;
			default:
				*buf++ = c;
				break;
		}
	}
	return STATE_CONTINUE;
}

int
state_field(struct state_t *state, char *buf, size_t nbytes)
{
	int term = 0;
	char c;

	while (state->len-- > 0 && state->read++ < nbytes) {
		c = *state->buf++;
		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
			default:
				*buf++ = c;
				term = 0;
				break;
		}
	}
	return STATE_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////////

void
http_parser_init(http_parser *parser)
{
	parser->state.state = HTTP_DATA_METHOD;
	parser->state.next = state_method;
	parser->state.len = 0;
	parser->state.read = 0;
}

void
http_parser_set_buffer(http_parser *parser, const char *buf, size_t nbytes)
{
	parser->state.buf = buf;
	parser->state.len = nbytes;
}

int
http_parser_next_event(http_parser *parser, char *buf, size_t nbytes, event_data_t *evt)
{
	int event = -1;

	if (parser->state.len > 0 && parser->state.next) {
		switch (parser->state.next(&parser->state, buf, nbytes)) {
			case STATE_ACCEPT:
				break;
			case STATE_FAIL:
				break;
			case STATE_CONTINUE:
				break;
			default:
				break;
		}
	}

	return event;
}
