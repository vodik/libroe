#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

//#include <string.h>

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
	STATE_METHOD,
	STATE_PATH,
	STATE_VERSION,
	STATE_HEADER,
	STATE_FIELD,

	LAST_STATE,
};

typedef int state_fn(IO *io, struct string *dest);

////////////////////////////////////////////////////////////////////////////////

state_fn state_method, state_path, state_version, state_header, state_field;

static state_fn *States[LAST_STATE] = {
	[STATE_METHOD]  = &state_method,
	[STATE_PATH]    = &state_path,
	[STATE_VERSION] = &state_version,
	[STATE_HEADER]  = &state_header,
	[STATE_FIELD]   = &state_field,
};

static int Transforms[LAST_STATE] = {
	[STATE_METHOD]  = STATE_PATH,
	[STATE_PATH]    = STATE_VERSION,
	[STATE_VERSION] = STATE_HEADER,
	[STATE_HEADER]  = STATE_FIELD,
	[STATE_FIELD]   = STATE_HEADER,
};

enum {
	STATE_ACCEPT,
	STATE_FAIL,
	STATE_DONE,
};

int
state_method(IO *io, struct string *dest)
{
	char c;

	while (!io_eof(io)) {
		c = io_getc(io);

		if (c == ' ')
			return STATE_ACCEPT;
		else if (c >= 'A' && c <= 'Z')
			string_putc(dest, c);
		else
			return STATE_FAIL;
	}
	return STATE_FAIL;
}

int
state_path(IO *io, struct string *dest)
{
	char c;

	while (!io_eof(io)) {
		c = io_getc(io);

		if (c == ' ')
			return STATE_ACCEPT;
		else if (c >= 'A' && c <= 'Z')
			string_putc(dest, c);
		else
			return STATE_FAIL;
	}
	return STATE_FAIL;
}

int
state_version(IO *io, struct string *dest)
{
	int term = 0;
	char c;

	while (!io_eof(io)) {
		c = io_getc(io);

		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
				return STATE_FAIL;
			default:
				string_putc(dest, c);
				term = 0;
				break;
		}
	}
	return STATE_FAIL;
}

int
state_header(IO *io, struct string *dest)
{
	int term1 = 0, term2 = 0;
	int read = 0;
	char c;

	while (!io_eof(io)) {
		c = io_getc(io);
		++read;

		switch (c) {
			case '\r':
				if (read == 1)
					term2 = 1;
				break;
			case '\n':
				if (read == 2 && term2 == 1)
					return STATE_DONE;
				break;
			case ':':
				term1 = 1;
				break;
			case ' ':
				if (term1)
					return STATE_ACCEPT;
				return STATE_FAIL;
			default:
				string_putc(dest, c);
				term1 = 0;
				break;
		}
	}
	return STATE_FAIL;
}

int
state_field(IO *io, struct string *dest)
{
	int term = 0;
	char c;

	while (!io_eof(io)) {
		c = io_getc(io);

		switch (c) {
			case '\r':
				term = 1;
				break;
			case '\n':
				if (term)
					return STATE_ACCEPT;
				return STATE_FAIL;
			default:
				string_putc(dest, c);
				term = 0;
				break;
		}
	}
	return STATE_FAIL;
}

////////////////////////////////////////////////////////////////////////////////

struct request *
parse_request(IO *io)
{
	struct request *request = malloc(sizeof(struct request));
	struct string *dest;
	int state = STATE_METHOD;
	state_fn *func = States[state];
	int ret;

	request->method = string_new(0);
	request->path = string_new(0);
	request->version = string_new(0);
	request->headers = hashtable_new(23, NULL);

	dest = request->method;
	while ((ret = func(io, dest)) != STATE_DONE) {
		if (ret == STATE_FAIL) {
			printf("invalid state!, TODO: handle this ;)\n");
			exit(EXIT_FAILURE);
		}
		printf("dest: %s\n", string_raw(dest));
		string_clear(dest);
		state = Transforms[state];
	}

	return request;
}

void
request_free(struct request *request)
{
	hashtable_free(request->headers, free);
	free(request);
}

