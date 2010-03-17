#include "smallhttp.h"

#include <stdlib.h>
#include <stdio.h>
#include "server.h"

int readsocket(int id, int fd)
{
	fprintf(stderr, "got a connection\n");
	return 1;
}

int smallhttp_main(int argc, char *argv[], httpd *ops)
{
	int port = argc > 1 ? atoi(argv[1]) : 8080;

	sockserver *srv = startserver(port);
	while (1) {
		pollserver(srv, 0, readsocket);
	}
}
