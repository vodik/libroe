#ifndef SMALLHTTP_MPLEXSOCKS
#define SMALLHTTP_MPLEXSOCKS

#include "util.h"

#define MAXCLIENTS 5

typedef struct mplexsocks mplexsocks;
typedef int (*recvcb)(void *, int);

struct mplexsocks {
	int fd;
	struct {
		int fd;
		void *context;
	} clients[MAXCLIENTS];

	void *(*onconnect)();
	void (*onclose)(void *);
};

int mplexsocks_init(int port, int concurrent, mplexsocks *);
int mplexsocks_poll(mplexsocks *, int timeout, recvcb callback, char **errmsg);
int mplexsocks_close(mplexsocks *);

const char *mplexsocks_errmsg(mplexsocks *);

#endif
