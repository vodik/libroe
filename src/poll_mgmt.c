#include "poll_mgmt.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <assert.h>

#include "util.h"
#include "config.h"

struct fd_evt_t {
	int fd;
	int type;
	const fd_cbs_t *cbs;
	conn_t *conn;
	void *shared;
};

/* FIXME: tie all connections to the service */

/** 
* @brief Set a socket so it won't block
* 
* @param fd The file descriptor of the socket
*/
static inline
void socket_set_nonblock(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
		die("fcntl getfd failed\n");
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
		die("fcntl setfd failed\n");
}

/** 
* @brief Set a socket to reuse its addr
* 
* @param fd The file descriptor of the socket
* @param state State to set SO_REUSEADDR to; one activates, zero deactivates.
*/
static inline
void socket_set_reuseaddr(int fd, int state)
{
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) < 0)
		die("setsockopt SO_REUSEADDR failed\n");
}

/** 
* @brief Helper function so skipset can cleanup remaining connections on shutdown.
* 
* @param arg A pointer to fd_event_t, stored in poll_mgmt_t store.
*/
static void
fd_cleanup(void *arg)
{
	struct fd_evt_t *data = arg;

	if (data->type != CONN_LISTENING) {
		if (data->cbs && data->cbs->onclose)
			data->cbs->onclose(data->conn);
		conn_close(data->conn);
	}

	close(data->fd);
	free(arg);
}

//////////////////////////////////////////////////////////////////////////////

/** 
* @brief Generate and store auxiliary connection data associated with a socket.
* 
* @param mngr Self pointer.
* @param fd The file descriptor of the socket
* @param type The typer of connection: listening or otherwise.
* @param cbs Callbacks to onopen, onmessage, onclose
* @param shared A pointer to data shared by all connections.
* 
* @return A pointer to the store, free to be associated with the epolling event
* data structure.
*/
static struct fd_evt_t *
poll_mgmt_mkstore(poll_mgmt_t *mngr, int fd, int type, const fd_cbs_t *cbs, void *shared)
{
	struct fd_evt_t *data = malloc(sizeof(struct fd_evt_t));
	data->fd = fd;
	data->type = type;
	data->cbs = cbs;
	data->shared = shared;

	/* we don't want a connection data type for listening connections <- FIXME: we do want service_conn_t */
	data->conn = type != CONN_LISTENING ? conn_new(cbs->cnfo.size, fd, cbs->cnfo.write, cbs->cnfo.destroy) : NULL;

	skipset_add(&mngr->store, fd, data);
	return data;
}

/** 
* @brief Remove auxiliary connection data associated with a socket.
* 
* @param mngr Self pointer.
* @param fd The file descriptor of the socket
*/
static inline void
poll_mgmt_removestore(poll_mgmt_t *mngr, int fd)
{
	struct fd_evt_t *data = skipset_remove(&mngr->store, fd);
	free(data);
}

/** 
* @brief Helper function to accept an incoming connection and store associated data.
* 
* @param mngr Self pointer.
* @param data Auxiliary connection information.
*/
static void
poll_mgmt_accept(poll_mgmt_t *mngr, struct fd_evt_t *data)
{
	struct epoll_event evt;
	int fd;

	fd = accept(data->fd, NULL, NULL);
	if (fd < 0)
		die("accept failed\n");

	printf("--> accept\n");
	socket_set_nonblock(fd);

	struct fd_evt_t *newdata = poll_mgmt_mkstore(mngr, fd, CONN_CONNECTION, data->cbs, data->shared);
	if (newdata->cbs && newdata->cbs->onopen)
		newdata->cbs->onopen(newdata->conn);

	evt.events = EPOLLIN;
	evt.data.ptr = newdata;

	if (epoll_ctl(mngr->fd, EPOLL_CTL_ADD, fd, &evt) == -1)
		die("epoll_ctr failed to add a connection\n");
}

/** 
* @brief Helper function to handle incoming data.
* 
* @param mngr Self pointer.
* @param data Auxiliary connection information.
*/
static void
poll_mgmt_handle(poll_mgmt_t *mngr, struct fd_evt_t *data)
{
	int keepalive = 0;

	printf("--> handling\n");

	if (data->cbs && data->cbs->onmessage)
		keepalive = data->cbs->onmessage(data->conn, data->shared);

	/* TODO: move this into its own function */
	if (keepalive == 0) {
		if (data->cbs && data->cbs->onclose)
			data->cbs->onclose(data->conn);

		printf("--> closing fd!\n");
		if (epoll_ctl(mngr->fd, EPOLL_CTL_DEL, data->fd, NULL) == -1)
			die("%d: epoll_ctl failed to remove connection\n", __LINE__);

		if (data->conn)
			conn_close(data->conn);

		close(data->fd);
		poll_mgmt_removestore(mngr, data->fd);
	}
}

//////////////////////////////////////////////////////////////////////////////

/** 
* @brief Start the polling subsystem.
* 
* @param mngr Self pointer.
* @param size Number of readable concurrent events. This does not set an upper limit
* to the number of concurrent connections but sets how many events can be read by one
* call to epoll_wait. See man epoll_create.
*/
void
poll_mgmt_start(poll_mgmt_t *mngr, int size)
{
	mngr->fd = epoll_create(size);
	mngr->size = size;
	mngr->events = malloc(sizeof(struct epoll_event) * size);
	skipset_init(&mngr->store, 5);
}

/** 
* @brief Stop the polling subsystem. This closes all outstanding open connections.
* 
* @param mngr Self pointer.
*/
void
poll_mgmt_stop(poll_mgmt_t *mngr)
{
	skipset_cleanup(&mngr->store, fd_cleanup);
	free(mngr->events);
	close(mngr->fd);
}

/** 
* @brief Start listening on a port and handle incoming connections based on a policy of callbacks.
* 
* @param mngr Self pointer.
* @param port The port to listen on.
* @param cbs A series of callbacks to use as a policy for handling incoming connections.
* @param shared Shared data that is assessable to callbacks.
* 
* @return The file descriptor of the listening socket.
*/
int
poll_mgmt_listen(poll_mgmt_t *mngr, int port, const fd_cbs_t *cbs, void *shared)
{
	struct sockaddr_in addr;
	struct epoll_event evt;
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
	if (listen(fd, ACCEPT_BACKLOG) == -1)
		die("listen on port %d failed\n", port);

	evt.events = EPOLLIN | EPOLLHUP;
	evt.data.ptr = poll_mgmt_mkstore(mngr, fd, CONN_LISTENING, cbs, shared);
	if (epoll_ctl(mngr->fd, EPOLL_CTL_ADD, fd, &evt) == -1)
		die("epoll_ctl failed to add listening connection on port %d\n", port);

	return fd;
}

/** 
* @brief Stop listening on a port. Not implemented yet as its not used, yet.
* 
* @param mngr Self pointer.
* @param fd The file descriptor of the listening port. Not final.
*/
void
poll_mgmt_close(poll_mgmt_t *mngr, int fd)
{
	die("poll_mgmt_close has not been implemented");
}

/* FIXME */
int done = 0;

static void
handler(int sig) { done = 1; }

/** 
* @brief Poll incoming connections or data and handle them appropriately.
* 
* @param mngr Self pointer.
* @param timeout A timeout to wait for something to happen in.
* 
* @return Return code. 0 if OK, > 1 if error.
*/
int
poll_mgmt_poll(poll_mgmt_t *mngr, int timeout)
{
	struct fd_evt_t *data;
	struct sigaction sa;
	sigset_t ss;
	int fd_count, i;

	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigemptyset(&ss);

	fd_count = epoll_pwait(mngr->fd, mngr->events, mngr->size, timeout, &ss);
	if (done == 1)
		return 1;
	else if (fd_count == -1)
		return errno == EINTR ? 0 : errno;

	for (i = 0; i < fd_count; ++i) {
		data = mngr->events[i].data.ptr;
		switch (data->type) {
			case CONN_LISTENING:
				poll_mgmt_accept(mngr, data);
				break;
			case CONN_CONNECTION:
				poll_mgmt_handle(mngr, data);
				break;
		}
	}
	return 0;
}
