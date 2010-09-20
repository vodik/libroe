#ifndef LIBS_CONN_REF
#define LIBS_CONN_REF

#include "parser.h"

struct conn;

struct service {
	char *name;
	int port;
	IO *io;
	int active;
	void (*cb)(struct service *service, struct conn *conn);
	const struct service_descrpt *descrpt;
	struct service *next;
};

struct conn {
	IO *io;
	struct service *service;
	unsigned refs;
	struct request *request;
};

#endif
