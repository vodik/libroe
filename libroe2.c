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

int
main(int argc, char *argv[])
{
	signal(SIGINT, end);

	http = roe_start("http", 9991);
	io_run();

	return 0;
}
