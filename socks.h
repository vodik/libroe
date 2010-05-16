#ifndef SMALLHTTP_SOCKS
#define SMALLHTTP_SOCKS

#define MAX_CONNECTIONS 10

typedef void *(*on_connect_cb)(int fd, void *arg);
typedef int (*on_recv_cb)(int fd, void *context, void *arg);
typedef void (*on_disconnect_cb)(void *context, void *arg);

enum {
	FD_CONNECTION,
	FD_LISTENING,
};

struct fdcbs_t {
	on_connect_cb		onconn;
	on_recv_cb			onrecv;
	on_disconnect_cb	ondisconn;
};

struct fde_t {
	int fd;
	int type;
	const struct fdcbs_t *callbacks;
	void *context, *arg;
};

struct epoll_t {
	int fd;
	struct fde_t table[MAX_CONNECTIONS + 4]; // TODO: update.
};

void epoll_init(struct epoll_t *s, int concurrent);
int epoll_listen(struct epoll_t *s, int port, const struct fdcbs_t *callbacks, void *arg);
void epoll_poll(struct epoll_t *s, int timeout);

#endif
