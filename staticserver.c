#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mplexsocks.h>
#include <http/parser.h>

struct http_context {
	http_parser parser;
};

static void *onconnect()
{
	struct http_context *context = malloc(sizeof(struct http_context));
	http_parser_init(&context->parser);
	return context;
}

static void onclose(void *ctxt)
{
	struct http_context *context = ctxt;

	http_parser_free(&context->parser);
	free(context);
}

/**
 * FIXME: there are memory allocation issues in the parser. */
static int onrecv(void *ctxt, int socketfd)
{
	struct http_context *context = ctxt;

	char buf[BUFSIZ];
	ssize_t r = 1;
	const http_request const *request;

	while (r > 0) {
		r = read(socketfd, buf, BUFSIZ);
		http_parser_read(&context->parser, buf, r);
	}
	request = http_parser_done(&context->parser);
	if (request) {
		printf("HTTP header:\n");
		printf(" > method:     %i\n", request->method);
		printf(" > path:       %s\n", request->path);
		printf(" > version:    HTTP/%i.%i\n", request->version_major, request->version_minor);
		printf(" > host:       %s\n", (char *)hashtable_get(&request->headers, "Host"));
		printf(" > user-agent: %s\n", (char *)hashtable_get(&request->headers, "User-Agent"));
		printf("\n");
		return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	mplexsocks socks;
	char *err = NULL;
	int port = 8080;
	int result;

	if (argc > 2) {
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else if (argc == 2) {
		port = atoi(argv[1]);
	}

	result = mplexsocks_init(port, 5, &socks);
	if (result) {
		fprintf(stderr, "couldn't start server: %s\n", mplexsocks_errmsg(&socks));
		mplexsocks_close(&socks);
		exit(EXIT_FAILURE);
	}

	socks.onconnect = onconnect;
	socks.onclose = onclose;

	while (1) {
		result = mplexsocks_poll(&socks, 0, onrecv, &err);
		if (result) {
			fprintf(stderr, "mplexsocks_poll failed, reason: %s", err);
			break;
		}
	}

	mplexsocks_close(&socks);
	return EXIT_SUCCESS;
}
