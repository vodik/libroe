#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "io.h"
#include "services.h"

struct service *http;

void
end(int sig)
{
	printf("\nShutting down!\n");
	roe_stop(http);
}

void
on_request(struct service *service, struct conn *conn)
{
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
