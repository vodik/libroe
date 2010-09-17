#ifndef LIBROE_CONN
#define LIBROE_CONN

struct conn;

struct conn *conn_new_fd(int fd);
struct conn *conn_ref(struct conn *conn);
void conn_close(struct conn *conn);

#endif
