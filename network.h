#ifndef LIBROE2_NETWORK
#define LIBROE2_NETWORK

void socket_set_nonblock(int fd);
void socket_set_reuseaddr(int fd, int state);

#endif
