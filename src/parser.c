#include <parser.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sbuf.h>

struct _temp {
	int fd;
	struct timeval tv;
};

int
pull_fd(void *dat, void *buf, size_t nbytes)
{
	struct _temp temp = dat;

	fd_set fds;
	struct timeval tv;
	int n;

	FD_ZERO(&fds);
	FD_SET(dat->fd, &fds);

	n = select(fd + 1, &fds, NULL, NULL, &dat->tv);
	if (n > 0) {
		if (FD_ISSET(fd, &fds)) {
			n = read(fd, buf, nbytes);
			return n;
		}
	}
	printf("--- timeout\n");
	return BUF_ERR;
}

////////////////////////////////////////////////////////////////////////////////

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

size_t
http_parser_read(http_parser *parser)
{
	size_t r = read(parser->state.buf, parser->state.len);
}

////////////////////////////////////////////////////////////////////////////////

void
http_parser_init(http_parser *parser, int fd)
{
	parser->state.state = HTTP_DATA_METHOD;
	parser->state.next = state_method;
	parser->state.len = 0;
	parser->state.read = 0;

	parser->fd = fd;
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
