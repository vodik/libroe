#include "http.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "io.h"
#include "network.h"
#include "util.h"

#include "parser.h"
#include "conn.h"
#include "conn_ref.h"

static void
http_hup(struct conn *conn)
{
	printf("--> hup\n");
	conn_close(conn);
}

static void
http_request(struct conn *conn)
{
	printf("--> request\n");

	if (!conn->request)
		conn->request = request_new();

	parse_request(conn->request, conn->io);

	conn->service->cb(conn->service, conn);
}

static void
http_incoming(IO *io, int events, void *arg)
{
	struct conn *conn = arg;

	if (events & IO_HUP)
		http_hup(conn);
	else if (events & IO_IN)
		http_request(conn);
}

IO *
http_init(int port)
{
	struct sockaddr_in addr;
	int fd;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		die("socket on port %d failed\n", port);

	socket_set_reuseaddr(fd, 1);
	socket_set_nonblock(fd);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		die("bind on port %d failed\n", port);
	if (listen(fd, 10) == -1)
		die("listen on port %d failed\n", port);

	return io_new_fd(fd);
}

void
http_accept(IO *io, int events, void *arg)
{
	int cfd, fd = io_get_fd(io);
	struct sockaddr_in addr = { 0 };
	size_t addr_len = sizeof(addr);
	IO *client;
	struct service *service = arg;

	if (events & IO_IN) {
		if ((cfd = accept(fd, (struct sockaddr *)&addr, &addr_len)) > -1) {
			socket_set_nonblock(cfd);
			client = io_new_fd(cfd);

			/* TODO: create conn */
			struct conn *conn = conn_new(service, io);
			conn->service = arg;

			printf("--> accepted\n");
			io_watch(client, IO_IN | IO_HUP, http_incoming, conn);
		}
	}
}
