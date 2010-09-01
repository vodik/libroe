#include "network.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "util.h"

void
socket_set_nonblock(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
		die("fcntl getfd failed\n");
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
		die("fcntl setfd failed\n");
}

void
socket_set_reuseaddr(int fd, int state)
{
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) < 0)
		die("setsockopt SO_REUSEADDR failed\n");
}
