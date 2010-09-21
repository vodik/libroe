#ifndef LIBROE_CONN
#define LIBROE_CONN

#include "io.h"

struct conn;
struct service;
struct request;

/*struct conn *conn_new_fd(int fd);
struct conn *conn_ref(struct conn *conn);
void conn_close(struct conn *conn);*/

struct conn *conn_new(struct service *service, IO *io);
void conn_close(struct conn *conn);

struct request *conn_request(struct conn *conn);

#endif
