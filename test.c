#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "io.h"
#include "services.h"
#include "conn.h"
#include "parser.h"
#include "response.h"

struct service *http;

__attribute__((noreturn)) void
end(int sig)
{
	printf("\nShutting down!\n");
	roe_stop(http);
	exit(0);
}

void
on_request(struct service *service, struct conn *conn)
{
	struct request *request = conn_request(conn);

	printf("REQEST!\n");
	printf(" > %s on %s\n", request->method, request->path);
	printf(" > HOST: %s\n", request_header(request, "Host"));
	printf(" > USER_AGENT: %s\n", request_header(request, "User-Agent"));

	if (strcmp("/", request->path) == 0) {
		respond_with_file(conn, "index.html");
	} else {
		respond_with_file(conn, request->path + 1);
	}

	conn_close(conn);
}

int
main(int argc, char *argv[])
{
	signal(SIGINT, end);

	http = roe_new("http", 9991);
	roe_start(http, on_request);

	io_run();

	return 0;
}
