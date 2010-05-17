#include "socks.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "util.h"

static inline void setnonblocking(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
		die("fcntl getfd failed\n");
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
		die("fcntl setfd failed\n");
}

void epoll_init(struct epoll_t *s, int concurrent)
{
	int i;
	for (i = 0; i < MAX_CONNECTIONS + 4; i++)
		s->table[i].fd = 0;

	s->fd = epoll_create(MAX_CONNECTIONS);
	if (s->fd == -1)
		die("epoll_create failed\n");
}

static struct fde_t *epolldata(struct epoll_t *s, int fd, int type, const struct fdcbs_t *callbacks, void *context, void *arg)
{
	struct fde_t *data = &s->table[fd];
	data->fd = fd;
	data->type = type;
	data->callbacks = callbacks;
	data->context = context;
	data->arg = arg;
	return data;
}

int epoll_listen(struct epoll_t *s, int port, const struct fdcbs_t *callbacks, void *arg)
{
	struct sockaddr_in addr = { 0 };
	struct epoll_event evt = { 0 };
	int reuse_addr = 1;
	int fd;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		die("socket on port %d failed\n", port);

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	setnonblocking(fd);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		die("bind on port %d failed\n", port);
	if (listen(fd, MAX_CONNECTIONS) == -1)
		die("listen on port %d failed\n", port);

	evt.events = EPOLLIN;
	evt.data.ptr = epolldata(s, fd, FD_LISTENING, callbacks, NULL, arg);
	if (epoll_ctl(s->fd, EPOLL_CTL_ADD, fd, &evt) == -1)
		die("epoll_ctl failed to add listening connection on port %d\n", port);
	
	return fd;
}

void epoll_close(struct epoll_t *s, int fd)
{
	if (epoll_ctl(s->fd, EPOLL_CTL_DEL, fd, NULL) == -1)
		die("%d: epoll_ctl failed to remove connection\n", __LINE__);
	close(fd);
	s->table[fd].fd = 0;
}

void epoll_stop(struct epoll_t *s)
{
	int i;
	struct fde_t *data;

	for (i = 0; i < MAX_CONNECTIONS + 4; ++i) {
		data = &s->table[i];
		if (data->fd == 0)
			continue;

		if (data->type == FD_CONNECTION) {
			if (data->callbacks && data->callbacks->ondisconn)
				data->callbacks->ondisconn(data->context, data->arg);
		}
		if (epoll_ctl(s->fd, EPOLL_CTL_DEL, data->fd, NULL) == -1)
			die("%d: epoll_ctl failed to remove connection\n", __LINE__);
		close(data->fd);
		data->fd = 0;
	}
	close(s->fd);
	s->fd = 0;
}

static void acceptconn(struct epoll_t *s, struct fde_t *data)
{
	struct epoll_event evt;
	int cfd;
	void *context = NULL;

	cfd = accept(data->fd, NULL, NULL);
	if (cfd < 0)
		die("accept failed\n");
	setnonblocking(cfd);

	if (data->callbacks && data->callbacks->onconn)
		context = data->callbacks->onconn(cfd, data->arg);

	evt.events = EPOLLIN | EPOLLHUP | EPOLLET | EPOLLRDHUP;
	evt.data.ptr = epolldata(s, cfd, FD_CONNECTION, data->callbacks, context, data->arg);
	if (epoll_ctl(s->fd, EPOLL_CTL_ADD, cfd, &evt) == -1)
		die("%d: epoll_ctr failed to add a connection\n");
}

static int handleconn(struct epoll_t *s, int event, struct fde_t *data)
{
	int close = 1;
	if (data->callbacks && data->callbacks->onrecv)
		close = data->callbacks->onrecv(data->fd, data->context, data->arg);
	return close;
}

static int done = 0;
static void handler(int sig) { done = 1;  }

int epoll_poll(struct epoll_t *s, int timeout)
{
	struct epoll_event events[MAX_CONNECTIONS];
	struct fde_t *data;
	int nfds, n;

	struct sigaction sa;
	sigset_t emptyset;

	sa.sa_handler = handler;        /* Establish signal handler */
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);

	sigemptyset(&emptyset);

	nfds = epoll_pwait(s->fd, events, MAX_CONNECTIONS, timeout, &emptyset);
	if (done == 1) {
		done = 0;
		return 1;
	}

	if (nfds == -1) {
		return errno == EINTR ? 0 : errno;
	}

	for (n = 0; n < nfds; ++n) {
		data = events[n].data.ptr;
		switch (data->type) {
			case FD_LISTENING:
				acceptconn(s, data);
				break;
			case FD_CONNECTION:
				if (handleconn(s, events[n].events, data)) {
					if (epoll_ctl(s->fd, EPOLL_CTL_DEL, data->fd, &events[n]) == -1)
						die("%d: epoll_ctl failed to remove connection\n", __LINE__);
					if (data->callbacks && data->callbacks->ondisconn)
						data->callbacks->ondisconn(data->context, data->arg);
					close(data->fd);
					data->fd = 0;
				}
				break;
		}
	}
	return 0;
}
