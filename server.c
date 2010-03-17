#include "server.h"

#define MAXCLIENTS 5

struct {
	int fd;
	
	int nextid;
	struct {
		int id;
		int fd;
	} clients[MAXCLIENTS];

	fd_set fdset;
	int highfd;

	log_t *log;
} sockserver;

static void setnonblocking(int sock);

static void handleconn(sockserver *);
static void closeconn(sockserver *, int i);
static void readsocks(sockserver *, ready_cb callback);

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
	fd_set *fdset = &srv->fdset;

	conn = accept(srv->fd, NULL, NULL);
	if (conn < 0)
		die("accept failed\n");

	setnonblocking(conn);

	for (i = 0; i < 5; i++) {
		if (srv->clients[i]->fd == 0) {
			log(serverlog, "connection accepted\n");
			srv->clients[i]->fd = conn;
			srv->clients[i]->id = s->nid++;

			FD_SET(conn, &srv->fdset);
			if (conn > srv->highfd)
				srv->highfd = conn;
			return;
		}
	}

	log(serverlog, "connection rejected: no room left for new clients\n");
	close(conn);
}

void closeconn(sockserver *srv, int i)
{
	int i, fd = srv->clients[i]->fd;
	srv->clients[i]->fd = 0;

	close(fd);
	FD_CLEAR(fd, &srv->fdset);

	if (fd == srv->highfd) {
		srv->highfd = srv->clients[0]->fd;

		for (i = 1; i < MAXCLIENTS; i++) {
			fd = srv->clients[i]->fd;
			if (fd > srv->highfd)
				srv->highfd = fd;
		}
	}
}

void readsocks(sockserver *svr, ready_cb callback)
{
	int i;

	if (FD_ISSET(srv->fd, &socks))
		handleconn(srv);

	/* TODO: call callback, determine if needs to close */
	for (i = 0; i < MAXCLIENTS; i++) {
		if (FD_ISSET(srv->clients[i]->fd, &srv->fdset)) {
			if (callback(srv->clients[i]->id, srv->clients[i]->fd))
				closeconn(srv, i);
		}
	}
}

sockserver *startserver(int port)
{
	socksever *srv = malloc(sizeof(sockserver));

	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);
	int reuse_addr = 1;
	int result;

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

	result = bind(srv->fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rslt == -1)
		die("bind failed on port %d\n", port);

	result = listen(srv->fd, MAXCLIENTS);
	if (rslt == -1)
		die("listen failed on port %d\n", port);

	srv->highfd = srv->fd;
	FD_ZERO(&srv->fdset);
	memset(&srv->clients, 0, sizeof(srv->clients));
}

void pollserver(sockserver *srv, int timeout, ready_cb callback);
{
	struct sockaddr_in caddr;
	unsigned int addrlen = sizeof(struct sockaddr_in);
	struct timeval* tmout = NULL;
	int cfd = 0;
	int ready;

	if (!srv || !srv->fd)
		return;

	if (timeout > 0) {
		tmout = malloc(sizeof(struct timeval));
		tmout->tv_sec = timeout / 1000;
		tmout->tv_usec = (timeout % 1000) * 1000;
	}

	if (ready = select(srv->highfd + 1, &srv->fdset, 0, 0, tmout)) {
		if (ready < 0)
			die("select failed\n");
		else if (ready == 0) {
			printf(".");
			fflush(stdout);
		}
		else
			readsocks(srv, callback);
	}

	if (tmout)
		free(tmout);
}
