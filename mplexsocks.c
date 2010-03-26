#include "mplexsocks.h"

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

static inline void setnonblocking(int fd);
static void acceptconn(mplexsocks *);
static inline void closeconn(mplexsocks *, int index);
static void readsocks(mplexsocks *socks, fd_set *set, recvcb callback);
static inline struct timeval *gettimeout(int timeout);
static inline int buildfdset(mplexsocks *socks, fd_set *set);

/**
 * an internal function to set a file descriptor to not block
 * @param fd the file descriptor
 */
static inline void setnonblocking(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
		die("fcntl getfd failed\n");
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
		die("fcntl setfl failed\n");
}

/**
 * an internal function which accepts an incoming connection and
 * adds it to client list.
 * @param socks a self pointer
 */
static void acceptconn(mplexsocks *socks)
{
	int i, cfd = accept(socks->fd, NULL, NULL);
	if (cfd < 0)
		die("accept failed\n");

	setnonblocking(cfd);

	for (i = 0; i < MAXCLIENTS; ++i) {
		if (socks->clients[i].fd == 0) {
			socks->clients[i].fd = cfd;
			socks->clients[i].context = (socks->onconnect ? socks->onconnect() : NULL);
			return;
		}
	}

	/* on error, we get here */
	close(cfd);
	die("connection rejected due to lack of space\n");
}

/**
 * an internal function which closes a managed connection.
 * @param socks a self pointer
 * @param index the index to the connection to close
 */
static inline void closeconn(mplexsocks *socks, int index)
{
	if (socks->onclose)
		socks->onclose(socks->clients[index].context);

	close(socks->clients[index].fd);
	socks->clients[index].fd = 0;
}

/**
 * an internal function to determine handle the work on the
 * fd set after a successful select. If the server fd is set,
 * try to accept a new connection, otherwise call the callback
 * with the set client fd.
 * @param socks a set pointer
 * @param set the fd_set
 * @param callback the callback to fire
 */
static void readsocks(mplexsocks *socks, fd_set *set, recvcb callback)
{
	int i;

	if (FD_ISSET(socks->fd, set))
		acceptconn(socks);
	for (i = 0; i < MAXCLIENTS; ++i) {
		if (FD_ISSET(socks->clients[i].fd, set)) {
			if (!callback(socks->clients[i].context, socks->clients[i].fd))
				closeconn(socks, i);
		}
	}
}

/**
 * an internal function which to generate a timval struct.
 * @param timeout the timeout in seconds
 * @return A timval struct
 */
static inline struct timeval *gettimeout(int timeout)
{
	struct timeval *tmout = malloc(sizeof(struct timeval));
	tmout->tv_sec = timeout / 1000;
	tmout->tv_usec = (timeout % 1000) * 1000;
	return tmout;
}

/**
 * an internal function which builds up an fdset for mplexsocks
 * @param socks a self pointer
 * @param set the fd_set
 * @return the highest file descriptor
 */
static inline int buildfdset(mplexsocks *socks, fd_set *set)
{
	int i;
	int fd; int highfd = socks->fd;

	FD_ZERO(set);
	FD_SET(socks->fd, set);
	for (i = 0; i < MAXCLIENTS; i++) {
		fd = socks->clients[i].fd;
		if (fd > 0) {
			FD_SET(fd, set);
			if (fd > highfd)
				highfd = fd;
		}
	}

	return highfd;
}

/**
 * initialize a multiplexing socket manager.
 * @param port the port to listen on
 * @param concurrent currently unused, this is the number of concurrent connects to manage
 * @param socks a self pointer
 * @return return code
 */
int mplexsocks_init(int port, int concurrent, mplexsocks *socks)
{
	memset(socks->clients, 0, sizeof(socks->clients));

	struct sockaddr_in addr;
	int reuse_addr = 1;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	socks->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socks->fd < 0)
		die("socket failed\n");

	setsockopt(socks->fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	setnonblocking(socks->fd);

	if (bind(socks->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		die("bind failed on port %d\n", port);
	if (listen(socks->fd, MAXCLIENTS) == -1)
		die("listen failed on port %d\n", port);

	return 0;
}

/**
 * poll a multiplexing socket manager for incoming data.
 * @param socks a self pointer
 * @param timeout the timeout to call select with
 * @param callback the callback to call when a client connection can be read
 * @param errmsg an error message if failed, otherwise should be NULL
 * @return return code
 */
int mplexsocks_poll(mplexsocks *socks, int timeout, recvcb callback, char **errmsg)
{
	struct timeval *tmout = NULL;
	int ready;
	int highfd;
	fd_set fdset;

	if (!socks || !socks->fd)
		die("mplexsocks_poll called on uninitialized data\n");

	if (timeout > 0)
		tmout = gettimeout(timeout);

	highfd = buildfdset(socks, &fdset);
	ready = select(highfd + 1, &fdset, 0, 0, tmout);
	if (ready < 0)
		die("select failed\n");
	else if (ready > 0)
		readsocks(socks, &fdset, callback);

	if (tmout)
		free(tmout);
	
	return 0;
}

/**
 * stop a multiplexing socket manager. This will close all connections
 * @param socks a self pointer
 */
int mplexsocks_close(mplexsocks *socks)
{
	int i;

	if (!socks || !socks->fd)
		die("mplexsocks_close called on uninitialized data\n");
	
	close(socks->fd);
	socks->fd = 0;
	for (i = 0; i < MAXCLIENTS; ++i)
		closeconn(socks, i);
	return 0;
}

/**
 * get the error message on a failure
 * @param socks a self pointer
 * @return a pointer to the error message
 * */
const char *mplexsocks_errmsg(mplexsocks *socks)
{
	return NULL;
}
