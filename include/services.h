#ifndef LIBROE_SERVICES
#define LIBROE_SERVICES

#include <poll_mgmt.h>
#include <hashtable.h>
#include <parser.h>

enum {
	CONN_CLOSE,
	CONN_KEEP_ALIVE,
};

typedef struct {
	char *method;
	char *path;
	char *version;
	hashtable_t headers;
} request_t;

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int fd;
	poll_mgmt_t *mgmt;
} service_t;

int pull_request(request_t *request, parser_t *parser);

int roe_start(const char *name, poll_mgmt_t *mgmt, int port, const void *iface);
void roe_stop();

#endif
