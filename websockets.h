#ifndef LIBROE2_WS
#define LIBROE2_WS

#include "io.h"

struct io *ws_init(int port);
void ws_iofunc(struct io *io, int events, void *arg);

#endif
