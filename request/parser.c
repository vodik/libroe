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

void request_parser_init(request_parser *parser)
{
	//parser->buffer = malloc(bufsize);
	parser->state.next = state_method;
	parser->state.len = 0;
}

void request_parser_set_buffer(request_parser *parser, char *buf, size_t nbytes)
{
	parser->state.buf = buf;
	parser->state.len = nbytes;
}

int request_parser_next_event(request_parser *parser, char *buf, size_t nbytes, event_data_t *evt)
{
	//request_parser_event *event = &parser->event;
	int event = -1;

	if (parser->state.len > 0 && parser->state.next)
		event = parser->state.next(&parser->state, buf, nbytes, evt);

	return event;
}

////////////////////////////////////////////////////////////////////////////////

/*static const char test[] =
	"GET / HTTP/1.1\r\n"
	"Fuck: off\r\n"
	"Simon: is sooo awsome\r\n"
	"Tyler: is a dick\r\n"
	"\r\n";

int main(int argc, char *argv[])
{
	char buf[1024];
	char *temp = strdup(test);

	request_parser parser;
	event_data_t data;
	int read;

	request_parser_init(&parser);
	request_parser_set_buffer(&parser, temp, strlen(temp));
	printf("strlen: %d\n", strlen(temp));
	
	while ((read = request_parser_next_event(&parser, buf, 1024, &data)) > 0) {
		buf[read] = '\0';
		switch (data.type) {
			case HTTP_DATA_METHOD:
				printf("we got a method!\n");
				break;
			case HTTP_DATA_PATH:
				printf("we got a path!\n");
				break;
			case HTTP_DATA_VERSION:
				printf("we got version information!\n");
				break;
			case HTTP_DATA_HEADER:
				printf("we got a header\n");
				break;
			case HTTP_DATA_FIELD:
				printf("we got a field\n");
				break;
			default:
				printf("???\n");
		}
		printf("read: %d\nbuf: %s\n\n", read, buf);
	}
	return 0;
}*/
