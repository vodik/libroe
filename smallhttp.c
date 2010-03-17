#include "smallhttp.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"

int readsocket(int id, int fd)
{
	char buf[2048];
	int r = read(fd, buf, 2048);
	buf[r] = '\0';

	printf("%d: %s", id, buf);

	if (r == 0)
		return 1;
	return 0;
}

int smallhttp_main(int argc, char *argv[], httpd *ops)
{
	int port = argc > 1 ? atoi(argv[1]) : 8080;

	sockserver *srv = startserver(port);
	while (1)
		pollserver(srv, 0, readsocket);
}
