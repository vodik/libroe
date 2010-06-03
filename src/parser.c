#include <parser.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#include <ibuf.h>
#include <sbuf.h>

int
pull_tm_conn(void *arg, void *buf, size_t nbytes)
{
	struct ibuf_store *dat = arg;

	fd_set fds;
	int n;

	FD_ZERO(&fds);
	FD_SET(dat->fd, &fds);

	n = select(dat->fd + 1, &fds, NULL, NULL, &dat->tv);
	if (n > 0) {
		if (FD_ISSET(dat->fd, &fds)) {
			n = read(dat->fd, buf, nbytes);
			return n;
		}
	}
	printf("--- timeout\n");
	return BUF_TIMEOUT;
}

////////////////////////////////////////////////////////////////////////////////

state_fn state_method, state_path, state_version, state_header, state_field;

static state_fn *States[LAST_HTTP_DATA] = {
	[HTTP_DATA_METHOD]  = &state_method,
	[HTTP_DATA_PATH]    = &state_path,
	[HTTP_DATA_VERSION] = &state_version,
	[HTTP_DATA_HEADER]  = &state_header,
	[HTTP_DATA_FIELD]   = &state_field,
};

static int Transforms[LAST_HTTP_DATA] = {
	[HTTP_DATA_METHOD]  = HTTP_DATA_PATH,
	[HTTP_DATA_PATH]    = HTTP_DATA_VERSION,
	[HTTP_DATA_VERSION] = HTTP_DATA_HEADER,
	[HTTP_DATA_HEADER]  = HTTP_DATA_FIELD,
	[HTTP_DATA_FIELD]   = HTTP_DATA_HEADER,
};

enum {
	STATE_ACCEPT,
	STATE_FAIL,
	STATE_TIMEOUT,
};

int
state_method(struct state_t *state)
{
	ibuf_t *b = &state->buf;
	sbuf_t *d = &state->dest;
	char c;

	while (!ibuf_eof(b)) {
		c = ibuf_getc(b);

		if (c == ' ')
			return STATE_ACCEPT;
		else if (c >= 'A' && c <= 'Z')
			sbuf_putc(d, c);
		else
			return STATE_FAIL;
	}
	return STATE_FAIL;
}

int
state_path(struct state_t *state)
{
	ibuf_t *b = &state->buf;
	sbuf_t *d = &state->dest;
	char c;

	while (!ibuf_eof(b)) {
		c = ibuf_getc(b);

		if (c == ' ')
			return STATE_ACCEPT;
		else
			sbuf_putc(d, c);
	}
	return STATE_FAIL;
}

int
state_version(struct state_t *state)
{
	int term = 0;
	ibuf_t *b = &state->buf;
	sbuf_t *d = &state->dest;
	char c;

	while (!ibuf_eof(b)) {
		c = ibuf_getc(b);

		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
			default:
				sbuf_putc(d, c);
				term = 0;
				break;
		}
	}
	return STATE_FAIL;
}

int
state_header(struct state_t *state)
{
	int term = 0;
	ibuf_t *b = &state->buf;
	sbuf_t *d = &state->dest;
	char c;

	while (!ibuf_eof(b)) {
		c = ibuf_getc(b);

		switch (c) {
			case ':':
				term = 1;
				break;
			case ' ':
				if (term)
					return STATE_ACCEPT;
			default:
				sbuf_putc(d, c);
				term = 0;
				break;
		}
	}
	return STATE_FAIL;
}

int
state_field(struct state_t *state)
{
	int term = 0;
	ibuf_t *b = &state->buf;
	sbuf_t *d = &state->dest;
	char c;

	while (!ibuf_eof(b)) {
		c = ibuf_getc(b);

		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
			default:
				sbuf_putc(d, c);
				term = 0;
				break;
		}
	}
	return STATE_FAIL;
}

////////////////////////////////////////////////////////////////////////////////

void
http_parser_init(http_parser *parser, conn_t *conn, size_t size, int timeout)
{
	parser->state.state = HTTP_DATA_METHOD;
	parser->store.fd = conn->fd;
	parser->store.tv.tv_sec = timeout;
	parser->store.tv.tv_usec = 0;

	ibuf_init(&parser->state.buf, size, &parser->store, pull_tm_conn);
	sbuf_init(&parser->state.dest, 0);
}

void http_parser_cleanup(http_parser *parser)
{
	ibuf_cleanup(&parser->state.buf);
	sbuf_cleanup(&parser->state.dest);
}

int http_parser_next(http_parser *parser, const char **buf, size_t *len)
{
	struct state_t *state = &parser->state;
	state_fn *func = States[state->state];
	int ret;

	static int _die = 0;
	if (_die == 15)
		return 0;
	/* check if done here */

	sbuf_clear(&parser->state.dest);

	ret = func(state);

	if (ret == STATE_ACCEPT) {
		ret = state->state;
		state->state = Transforms[state->state];
		printf("%%%%%% state: \"%s\"\n", parser->state.dest.buf);
		++_die;
		return ret;
	}
	else
		return 0;

	/*switch (ret) {
		case STATE_ACCEPT:
			ret = state->state;
			state->state = Transforms[state->state];
			return ret;
		case STATE_TIMEOUT:
			return HTTP_EVT_TIMEOUT;
		case STATE_FAIL:
		default:
			return HTTP_EVT_ERROR;
	}*/
}
