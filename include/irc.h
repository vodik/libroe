#ifndef LIBROE_IRC
#define LIBROE_IRC

#include <conn.h>
#include <services.h>

typedef struct _irc {
	conn_t *base;
} irc_t;

////////////////////////////////////////////////////////////////////////////////

/* TODO move to a private header */
void irc_on_open(conn_t *conn);
int irc_on_message(conn_t *conn, const void *data);
void irc_on_close(conn_t *conn);

void irc_destroy(conn_t *conn);
size_t irc_write(conn_t *conn, const char *msg, size_t nbytes);

static const fd_cbs_t irc_callbacks = {
	.cnfo = {
		.size = sizeof(irc_t),
		.destroy = irc_destroy,
		.write = irc_write,
	},

	.onopen       = irc_on_open,
	.onmessage    = irc_on_message,
	.onclose      = irc_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int port;
} irc_iface_t;

#endif
