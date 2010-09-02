#ifndef LIBS_WATCH
#define LIBS_WATCH

#include <sys/epoll.h>

struct io;

enum io_events {
	IO_IN  = EPOLLIN,
	IO_OUT = EPOLLOUT,
	IO_HUP = EPOLLRDHUP,
};

typedef void (*iofunc)(struct io *, int, void *);

void io_watch(struct io *io, int events, iofunc func, void *arg);
void io_unwatch(struct io *io);

void io_poll(int timeout);
void io_run();

#endif
