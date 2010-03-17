#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "util.h"

#define MAXCLIENTS 5

struct sockserver {
	int fd;
	
	int nextid;
	struct {
		int id;
		int fd;
	} clients[MAXCLIENTS];

	log_t *log;
};

static void setnonblocking(int sock);

static void handleconn(sockserver *);
static void closeconn(sockserver *, int i);
static void readsocks(sockserver *srv, fd_set fdset, ready_cb callback);

void setnonblocking(int sock)
{
	int opts = fcntl(sock, F_GETFL);
	if (opts < 0)
		die("fcntl getfl failed\n");

	opts |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0)
		die("fcntl setfl failed\n");
}

void handleconn(sockserver *srv)
{
	int i, conn;

	conn = accept(srv->fd, NULL, NULL);
	if (conn < 0)
		die("accept failed\n");

	setnonblocking(conn);

	for (i = 0; i < MAXCLIENTS; i++) {
		if (srv->clients[i].fd == 0) {
			srv->clients[i].fd = conn;
			srv->clients[i].id = srv->nextid++;
			log(srv->log, "connection accepted | id: %d\n", srv->clients[i].id);
			return;
		}
	}

	log(srv->log, "connection rejected: no room left for new clients\n");
	close(conn);
}

void closeconn(sockserver *srv, int i)
{
	log(srv->log, "closing connection  | id: %d\n", srv->clients[i].id);

	close(srv->clients[i].fd);
	srv->clients[i].fd = 0;
}

void readsocks(sockserver *srv, fd_set fdset, ready_cb callback)
{
	int i;

	if (FD_ISSET(srv->fd, &fdset))
		handleconn(srv);

	/* TODO: call callback, determine if needs to close */
	for (i = 0; i < MAXCLIENTS; i++) {
		if (FD_ISSET(srv->clients[i].fd, &fdset)) {
			if (callback(srv->clients[i].id, srv->clients[i].fd))
				closeconn(srv, i);
		}
	}
}

sockserver *startserver(int port)
{
	sockserver *srv = malloc(sizeof(sockserver));
	memset(&srv->clients, 0, sizeof(srv->clients));

	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);
	int reuse_addr = 1;

	srv->log = newlog("/tmp/smallhttp-server.log");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	srv->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0)
		die("socket failed\n");

	setsockopt(srv->fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	setnonblocking(srv->fd);

	if (bind(srv->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		die("bind failed on port %d\n", port);

	if (listen(srv->fd, MAXCLIENTS) == -1)
		die("listen failed on port %d\n", port);

	srv->nextid = 0;
	return srv;
}

void pollserver(sockserver *srv, int timeout, ready_cb callback)
{
	struct sockaddr_in caddr;
	unsigned int addrlen = sizeof(struct sockaddr_in);
	struct timeval* tmout = NULL;
	int cfd = 0;
	int i, ready;
	int fd, highfd;
	fd_set fdset;

	if (!srv || !srv->fd)
		return;

	if (timeout > 0) {
		tmout = malloc(sizeof(struct timeval));
		tmout->tv_sec = timeout / 1000;
		tmout->tv_usec = (timeout % 1000) * 1000;
	}

	highfd = srv->fd;

	FD_ZERO(&fdset);
	FD_SET(srv->fd, &fdset);
	for (i = 0; i < MAXCLIENTS; i++) {
		fd = srv->clients[i].fd;
		if (fd > 0) {
			FD_SET(fd, &fdset);
			if (fd > highfd)
				highfd = fd;
		}
	}

	ready = select(highfd + 1, &fdset, 0, 0, tmout);
	if (ready < 0)
		die("select failed\n");
	else if (ready > 0)
		readsocks(srv, fdset, callback);

	if (tmout)
		free(tmout);
}
