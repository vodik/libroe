#ifndef LIBROE2_HTTP
#define LIBROE2_HTTP

#include "io.h"

struct io *http_init(int port);
void http_accept(struct io *io, int events, void *arg);

#endif
