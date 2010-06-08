#include <websocket.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include <sbuf.h>
#include <parser.h>
#include <util.h>

void
ws_on_open(conn_t *conn)
{
	printf("==> WS START: %d\n", conn->fd);
	ws_t *wsc = (ws_t *)conn;

	wsc->auth = false;
}

int
ws_make_handshake(ws_t *conn, request_t *request)
{
	sbuf_t response;
	static const char template[] =
		"HTTP/1.1 %d %s\r\n"
		"Upgrade: WebSocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Origin: http://%s\r\n"
		"Sec-WebSocket-Location: ws://%s%s\r\n";

	if (hashtable_get(&request->headers, "Sec-WebSocket-Key1") || hashtable_get(&request->headers, "Sec-WebSocket-Key2"))
		die("formal not supported\n");

	char *host = hashtable_get(&request->headers, "Host");
	char *path = request->path;

	assert(host);
	assert(path);
	
	sbuf_init(&response, 0);
	sbuf_scatf(&response, template, 101, "Websocket Protocal Handshake", host, host, path);
	sbuf_cat(&response, "\r\n");

	printf("==> SENDING HEADER:\n%s\n", sbuf_raw(&response));

	write(conn->base.fd, sbuf_raw(&response), sbuf_len(&response));
	sbuf_cleanup(&response);
	return 0;
}

int
ws_on_message(conn_t *conn, void *data)
{
	printf("==> WS MESSAGE: %d\n", conn->fd);

	ws_t *wsc = (ws_t *)conn;
	ws_iface_t *iface = (ws_iface_t *)data;

	if (wsc->auth == false) {
		printf("==> AUTHENTICATING WEBSOCKET\n");
		parser_t parser;
		int code;

		hashtable_init(&wsc->request.headers, 16, NULL);

		parser_init(&parser, conn, 512, 2);
		code = pull_request(&wsc->request, &parser);
		switch (code) {
			case HTTP_EVT_TIMEOUT:
				printf("==> TIMEOUT\n");
				return 0;
			case HTTP_EVT_ERROR:
				die("error");
				break;
		}

		ws_make_handshake(wsc, &wsc->request);
		if (iface && iface->onopen)
			iface->onopen(wsc, &wsc->request);

		wsc->auth = true;
		parser_cleanup(&parser);
	} else if (wsc->onmessage) {
		printf("==> HANDLEING WEBSOCKET MESSAGE\n");

		char buf[512];
		int r = read(conn->fd, buf, 512);
		buf[r] = '\0';
		printf("==> read: \"%s\" len: %d\n", buf, r);
		wsc->onmessage(wsc, buf, r);
	} else {
		printf("==> NO HANDLER INSTALLED\n");
		return 0;
	}

	return 1;
}

void
ws_on_close(conn_t *conn)
{
	printf("==> WS CLOSE: %d\n", conn->fd);
}

/******************************************************************************/

void
ws_destroy(conn_t *conn)
{
}

size_t
ws_write(conn_t *conn, const char *buf, size_t nbytes)
{
	ws_t *wsconn = (ws_t *)conn;

	char _buf[nbytes + 2];
	char *b = _buf;
	int i;

	/* Websocket messages start with 0x00 and end with 0xff */
	*b++ = 0x00;
	for (i = 0; i < nbytes; ++i)
		*b++ = *buf++;
	*b = (char)0xff;

	return write(wsconn->base.fd, _buf, nbytes + 2);
}
