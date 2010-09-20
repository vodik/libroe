#ifndef LIBROE2_SERVICES
#define LIBROE2_SERVICES

struct service;
struct conn;

typedef void (*roe_cb)(struct service *service, struct conn *conn);

struct service *roe_new(const char *name, int port);

void roe_start(struct service *service, roe_cb cb);
void roe_stop(struct service *service);

#endif
