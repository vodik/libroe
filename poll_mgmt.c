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

#include "util.h"
#include "config.h"

static inline void socket_set_nonblock(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
		die("fcntl getfd failed\n");
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
		die("fcntl setfd failed\n");
}

static inline void socket_set_reuseaddr(int fd, int state)
{
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &state, sizeof(state)) < 0)
		die("setsockopt SO_REUSEADDR failed\n");
}

//////////////////////////////////////////////////////////////////////////////

static void *poll_mgmt_mkstore(poll_mgmt_t *mngr, int fd, int type, struct fd_cbs_t *cbs)
{
	struct fd_evt_t *data = malloc(sizeof(struct fd_evt_t));
	data->fd = fd;
	data->type = type;
	data->cbs = cbs;

	skipset_add(&mngr->store, fd, data);
	return data;
}

static inline void poll_mgmt_removestore(poll_mgmt_t *mngr, int fd)
{
	struct fd_evt_t *data = skipset_remove(&mngr->store, fd);
	free(data);
}

static void poll_mgmt_accept(poll_mgmt_t *mngr, struct fd_evt_t *data)
{
	struct epoll_event evt;
	int fd;

	fd = accept(data->fd, NULL, NULL);
	if (fd < 0)
		die("accept failed\n");

	printf("oh, its accepted\n");
	
	socket_set_reuseaddr(fd, 1);
	socket_set_nonblock(fd);

	/* FIXME: raise event here */

	evt.events = EPOLLIN;
	evt.data.ptr = poll_mgmt_mkstore(mngr, fd, CONN_CONNECTION, data->cbs);
	if (epoll_ctl(mngr->fd, EPOLL_CTL_ADD, fd, &evt) == -1)
		die("epoll_ctr failed to add a connection\n");
}

static void poll_mgmt_handle(poll_mgmt_t *mngr, struct fd_evt_t *data)
{
	char buf[1024 * 8];

	int r = read(data->fd, buf, 1024 * 8);
	if (r != 0) {
		printf("%d: oh my god!\n", data->fd);
		buf[r] = '\0';
		data->cbs->onmessage(buf, r);
	}

	printf("killing!\n");
	if (epoll_ctl(mngr->fd, EPOLL_CTL_DEL, data->fd, NULL) == -1)
		die("%d: epoll_ctl failed to remove connection\n", __LINE__);
	close(data->fd);
	poll_mgmt_removestore(mngr, data->fd);
}

//////////////////////////////////////////////////////////////////////////////

void poll_mgmt_start(poll_mgmt_t *mngr, int size)
{
	mngr->fd = epoll_create(size);
	mngr->size = size;
	mngr->events = malloc(sizeof(struct epoll_event) * size);
	skipset_init(&mngr->store, 5);
}

void poll_mgmt_stop(poll_mgmt_t *mngr)
{
	skipset_cleanup(&mngr->store, free);
	free(mngr->events);
	close(mngr->fd);
}

int poll_mgmt_listen(poll_mgmt_t *mngr, int port, struct fd_cbs_t *cbs)
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
	evt.data.ptr = poll_mgmt_mkstore(mngr, fd, CONN_LISTENING, cbs);
	if (epoll_ctl(mngr->fd, EPOLL_CTL_ADD, fd, &evt) == -1)
		die("epoll_ctl failed to add listening connection on port %d\n", port);
	
	return fd;
}

void poll_mgmt_close(poll_mgmt_t *mngr, int fd)
{
}

/* FIXME */
int done = 0;
static void handler(int sig) { done = 1; }

int poll_mgmt_poll(poll_mgmt_t *mngr, int timeout)
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
