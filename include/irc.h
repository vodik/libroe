#ifndef SMALLHTTP_WEBSOCKET
#define SMALLHTTP_WEBSOCKET

#include <conn.h>
#include <services.h>
#include <sbuf.h>

/* TODO move to a private header */
void irc_on_open(struct fd_context_t *context);
int irc_on_message(struct fd_context_t *context, const char *msg, size_t nbytes);
void irc_on_close(struct fd_context_t *context);

static const fd_cbs_t irc_callbacks = {
	.onopen		= irc_on_open,
	.onmessage	= irc_on_message,
	.onclose	= irc_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct _irc {
	conn_t *base;
	//sbuf_t *path;

	//void (*onmessage)(struct _ws *ws, const char *msg, size_t nbytes);
	//void (*onclose)(struct _ws *ws);
} irc_t;

#endif
