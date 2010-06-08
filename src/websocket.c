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

/* FIXME: generalize this because I just copied this over from
 * http_pull_request */
int
ws_pull_request(ws_t *conn, parser_t *parser)
{
	int code;
	const char *b;
	size_t len;
	static char *header;

	while ((code = parser_next(parser, &b, &len)) > 0) {
		switch (code) {
			case HTTP_DATA_METHOD:
				printf("==> METHOD: \"%s\"\n", b);
				conn->request.method = strndup(b, len);
				break;
			case HTTP_DATA_PATH:
				printf("==> PATH: \"%s\"\n", b);
				conn->request.path = strndup(b, len);
				break;
			case HTTP_DATA_VERSION:
				printf("==> VERSION: \"%s\"\n", b);
				conn->request.version = strndup(b, len);
				break;
			case HTTP_DATA_HEADER:
				printf("==> HEADER: \"%s\"\n", b);
				header = strndup(b, len);
				break;
			case HTTP_DATA_FIELD:
				printf("==> FIELD: \"%s\"\n", b);
				hashtable_add(&conn->request.headers, header, strndup(b, len));
				free(header);
				break;
		}
	}
	return code;
}

////////////////////////////////////////////////////////////////////////////////

void
ws_on_open(conn_t *conn)
{
	printf("==> WS START: %d\n", conn->fd);
}

int
ws_on_message(conn_t *conn, void *data)
{
	printf("==> WS MESSAGE: %d\n", conn->fd);

	ws_t *wsc = (ws_t *)conn;
	ws_iface_t *iface = (ws_iface_t *)data;

	parser_t parser;
	int code;

	hashtable_init(&wsc->request.headers, 16, NULL);

	parser_init(&parser, conn, 512, 2);
	code = ws_pull_request(wsc, &parser);
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			printf("==> TIMEOUT\n");
			return 0;
		case HTTP_EVT_ERROR:
			die("error");
			break;
	}

	/* handle headers here
	 * char *vale = hashtable_get("Connection");
	 * if (strcmp(value, "Close"))
	 * 		..
	 */

	response_init(&wsc->response, conn);
	printf("==> QUERY REQUEST!\n");
	if (iface && iface->onrequest)
		iface->onrequest(wsc, &wsc->request, &wsc->response);

	response_cleanup(&wsc->response);
	parser_cleanup(&parser);
	return 1;
}

void
ws_on_close(conn_t *conn)
{
	printf("==> WS CLOSE: %d\n", conn->fd);
}

/**********************************************************/

//void
//ws_handle_headers(const char *header, const char *field)
//{
	//if (strcmp(header, "Host") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Connection") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Upgrade") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Origin") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Sec-WebSocket-Key1") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Sec-WebSocket-Key2") == 0) {
		///* TODO */
	//} else if (strcmp(header, "Sec-WebSocket-Protocol") == 0) {
		///* TODO */
	//}
//}
//
//int
//ws_acknowledge(ws_t *ws)
//{
	//sbuf_t *response = sbuf_new(LENGTH(message));
	//int ret;
//
	//sbuf_cat(response, message);
//
	///* send reponse */
	//ret = write(ws->fd, _S(response), sbuf_len(response));
	//if (ret < 0)
		//die("websocket acknowledge failed\n");
	//return 0;
//}

///******************************************************************************/

void
ws_close(ws_t *ws)
{
	//close(ws->fd);
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
