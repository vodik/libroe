#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <request/request.h>
#include <util/hashtable.h>

static int get_method(struct state *state);
static int get_path(struct state *state);
static int get_version(struct state *state);
static int get_header(struct state *state);
static int get_value(struct state *state);
static int read_request1(struct state *state, const char **http, int *len);
static int read_request2(struct state *state, const char **http, int *len);
static int read_header(struct state *state, const char **http, int *len);
static int read_value(struct state *state, const char **http, int *len);
static inline void startstate(struct state *state, void *arg);

/** 
* @brief 
* 
* @param state
* 
* @return 
*/
static int get_method(struct state *state)
{
	http_request *request = state->arg;

	if (strncmp("HEAD", state->buf, state->len) == 0)
		request->method = HTTP_HEAD;
	if (strncmp("GET", state->buf, state->len) == 0)
		request->method = HTTP_GET;
	if (strncmp("POST", state->buf, state->len) == 0)
		request->method = HTTP_POST;
	if (strncmp("PUT", state->buf, state->len) == 0)
		request->method = HTTP_PUT;
	if (strncmp("DELETE", state->buf, state->len) == 0)
		request->method = HTTP_DELETE;
	if (strncmp("TRACE", state->buf, state->len) == 0)
		request->method = HTTP_TRACE;
	if (strncmp("OPTIONS", state->buf, state->len) == 0)
		request->method = HTTP_OPTIONS;
	if (strncmp("CONNECT", state->buf, state->len) == 0)
		request->method = HTTP_CONNECT;
	if (strncmp("PATCH", state->buf, state->len) == 0)
		request->method = HTTP_PATCH;

	state->parse = get_path;
	state->next = read_request1;
	return 0;
}

/** 
* @brief 
* 
* @param state
* 
* @return 
*/
static int get_path(struct state *state)
{
	http_request *request = state->arg;

	request->path = strndup(state->buf, state->len);

	state->parse = get_version;
	state->next = read_request2;
	return 0;
}

/** 
* @brief 
* 
* @param state
* 
* @return 
*/
static int get_version(struct state *state)
{
	http_request *request = state->arg;
	char match[] = "HTTP/";
	char *m = match, *c = state->buf;

	while (state->len > 0 && *m == *c) {
		--state->len;
		++m;
		++c;
	}

	if (state->len == 3 && c[1] == '.') {
		request->version_major = c[0] - '0';
		request->version_minor = c[2] - '0';

		state->parse = get_header;
		state->next = read_header;
		return 0;
	}
	/* error */
	return 1;
}

/** 
* @brief 
* 
* @param state
* 
* @return 
*/
static int get_header(struct state *state)
{
	state->tmp = strndup(state->buf, state->len);
	state->parse = get_value;
	state->next = read_value;
	return 0;
}

/** 
* @brief 
* 
* @param state
* 
* @return 
*/
static int get_value(struct state *state)
{
	http_request *request = state->arg;

	hashtable_insert(&request->headers, state->tmp, strndup(state->buf, state->len));
	free(state->tmp);
	state->parse = get_header;
	state->next = read_header;
	return 0;
}

/** 
* @brief 
* 
* @param state
* @param http
* @param len
* 
* @return 
*/
static int read_request1(struct state *state, const char **http, int *len)
{
	char *b = state->buf + state->len;
	int result;

	while ((*len)-- > 0 && (*b++ = *(*http)++) != ' ' && state->len++ < BUFFER_LENGTH);
	*--b = '\0';

	if (*len == 0)
		return 0;
	else {
		result = state->parse(state);
		state->len = 0;
		return result;
	}
	return 1;
}

/** 
* @brief 
* 
* @param state
* @param http
* @param len
* 
* @return 
*/
static int read_request2(struct state *state, const char **http, int *len)
{
	char *b = state->buf + state->len;
	int result;

	while ((*len)-- > 0 && (*b++ = *(*http)++) != '\r' && state->len++ < BUFFER_LENGTH);
	*--b = '\0';

	if (*len == 0)
		return 0;
	else if ((*len)-- >= 0 && *(*http)++ == '\n') {
		result = state->parse(state);
		state->len = 0;
		return result;
	}
	return 1;
}

/** 
* @brief 
* 
* @param state
* @param http
* @param len
* 
* @return 
*/
static int read_header(struct state *state, const char **http, int *len)
{
	char *b = state->buf + state->len;
	int result;

	/* if we hit the empty line of \r\n, we're done, the rest is the body */
	if (state->len == 0) {
		if (*len >= 2 && (*http)[0] == '\r' && (*http)[1] == '\n') {
			*len -= 2;
			*http += 2;
			state->next = 0;
			state->done = 1;
			return 0;
		}
	}

	while ((*len)-- > 0 && (*b++ = *(*http)++) != ':' && state->len++ < BUFFER_LENGTH);
	*--b = '\0';

	if (*len == 0)
		return 0;
	else if ((*len)-- >= 0 && *(*http)++ == ' ') {
		result = state->parse(state);
		state->len = 0;
		return result;
	}
	/* error */
	return 1;
}

/** 
* @brief 
* 
* @param state
* @param http
* @param len
* 
* @return 
*/
static int read_value(struct state *state, const char **http, int *len)
{
	char *b = state->buf + state->len;
	int result;

	while ((*len)-- > 0 && (*b++ = *(*http)++) != '\r' && state->len++ < BUFFER_LENGTH);
	*--b = '\0';

	if (*len == 0)
		return 0;
	else if ((*len)-- >= 0 && *(*http)++ == '\n') {
		result = state->parse(state);
		state->len = 0;
		return result;
	}
	return 1;
}

/** 
* @brief 
* 
* @param state
* @param arg
*/
static inline void startstate(struct state *state, void *arg)
{
	state->len = 0;
	state->next = read_request1;
	state->parse = get_method;
	state->arg = arg;
	state->done = 0;
}

/** 
* @brief 
* 
* @param parser
*/
void http_parser_init(http_parser *parser, int fd)
{
	http_request_init(&parser->request, fd);
	startstate(&parser->state, &parser->request);
}

/** 
* @brief 
* 
* @param parser
*/
void http_parser_free(http_parser *parser)
{
	http_request_free(&parser->request);
}

/** 
* @brief 
* 
* @param parser
* @param buf
* @param len
* 
* @return 
*/
int http_parser_read(http_parser *parser, const char *buf, int len)
{
	while (len > 0 && parser->state.next) {
		if (parser->state.next(&parser->state, &buf, &len)) {
			fprintf(stderr, "Error parsing http request\n");
			return 1;
		}
	}
	return 0;
}

/** 
* @brief 
* 
* @param parser
* 
* @return 
*/
const http_request const *http_parser_done(http_parser *parser)
{
	return parser->state.done ? &parser->request : NULL;
}
