#ifndef SMALLHTTP_HTTP
#define SMALLHTTP_HTTP

#include <services.h>
#include <response.h>
#include <parser.h>
#include <conn.h>

typedef struct _http {
	conn_t base;
	int keep_alive;

	const char *method;
	const char *path;
	const char *version;

	void (*onheader)(struct _http *conn, const char *header, const char *field);
	void (*makeresponse)(struct _http *conn);
} http_t;

////////////////////////////////////////////////////////////////////////////////

/* TODO move to a private header */
void http_on_open(conn_t *conn);
int http_on_message(conn_t *conn, void *data);
void http_on_close(conn_t *conn);

static const fd_cbs_t http_callbacks = {
	.conn_size  = sizeof(http_t),
	/* .data    = <-- FIXME
	 * .oninit  =
	 */
	.onopen		= http_on_open,
	.onmessage	= http_on_message,
	.onclose	= http_on_close,
};

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	int port;
	void (*onrequest)(http_t *conn);
} http_iface_t;

#endif
