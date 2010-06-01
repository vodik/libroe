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
	printf("--> opening\n");
}

int
ws_on_message(conn_t *conn, const char *msg, size_t nbytes)
{
	http_parser reader;
	const char *buf;
	size_t len;
	int code;

	http_parser_init(&reader, conn, 60);
	while ((code = http_parser_next(&reader, &buf, &len)) > 0) {
		/* use buf, len */
	}
	switch (code) {
		case HTTP_EVT_TIMEOUT:
			return 0;
		default:
			die("error");
	}
	http_parser_cleanup(&reader);

	return 1;
}

void
ws_on_close(conn_t *conn)
{
}

/**********************************************************/
//struct ws_context_t {
	//http_parser parser;
	//ws_t ws;
//
	//int auth;
	//int expected_event; /* FIXME: this should be in the parser itself, this is a kudge */
	//sbuf_t *method, *version;
	//sbuf_t *field, *header;
//
	//sbuf_t *response;
//};
//
//const char message[] =
	//"HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
	//"Upgrade: WebSocket\r\n"
	//"Connection: Upgrade\r\n";
	///*"WebSocket-Origin: http://localhost:11234\r\n"
	//"WebSocket-Location: ws://localhost:33456/service\r\n"*/
//
//void
//ws_on_open(struct fd_context_t *context)
//{
	//struct ws_context_t *ws_context = malloc(sizeof(struct ws_context_t));
	//http_parser_init(&ws_context->parser);
//
	//ws_context->ws.fd = context->fd;
//
	//context->data = ws_context;
	//context->context_gc = free; /* FIXME: no long adequate */
//}
//
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
//
//int
//ws_on_message(struct fd_context_t *context, const char *msg, size_t nbytes)
//{
	//static char buf[1024];
	//event_data_t data;
	//int read = 0;
//
	//struct ws_context_t *ws_context = context->data;
	//http_parser *parser = &ws_context->parser;
	//ws_t *ws = &ws_context->ws;
//
	//struct ws_iface *iface = context->shared;
//
	//if (!iface->onopen)
		//return CONN_CLOSE;
//
	//http_parser_set_buffer(parser, msg, nbytes);
//
	//if (!ws_context->auth) {
		//switch (ws_context->expected_event) {
			//case HTTP_DATA_METHOD:
				//read = http_parser_next_event(parser, buf, 1024, &data);
				//buf[read] = '\0';
				//assert(data.type == HTTP_DATA_METHOD);
				//ws_context->expected_event = HTTP_DATA_PATH;
//
				//sbuf_ncat(ws_context->method, buf, read);
//
				//if (sbuf_cmp(ws_context->method, "GET") != 0) {
					//fprintf(stderr, "--- WEBSOCKET CAN ONLY GET\n");
					//return CONN_CLOSE;
				//}
//
			//case HTTP_DATA_PATH:
				//read = http_parser_next_event(parser, buf, 1024, &data);
				//assert(data.type == HTTP_DATA_PATH);
				//ws_context->expected_event = HTTP_DATA_VERSION;
//
				//sbuf_ncat(ws->path, buf, read);
//
			//case HTTP_DATA_VERSION:
				//read = http_parser_next_event(parser, buf, 1024, &data);
				//assert(data.type == HTTP_DATA_VERSION);
				//ws_context->expected_event = HTTP_DATA_HEADER;
//
				///* use version info? */
				//sbuf_ncat(ws_context->version, buf, read);
//
			//case HTTP_DATA_HEADER:
				//read = http_parser_next_event(parser, buf, 1024, &data);
				//assert(data.type == HTTP_DATA_HEADER);
				//ws_context->expected_event = HTTP_DATA_FIELD;
//
				//sbuf_ncat(ws_context->header, buf, read);
//
			//case HTTP_DATA_FIELD:
				//read = http_parser_next_event(parser, buf, 1024, &data);
				//assert(data.type == HTTP_DATA_FIELD);
				//ws_context->expected_event = HTTP_DATA_HEADER;
//
				//sbuf_ncat(ws_context->field, buf, read);
//
				///* TODO: handle header/value pairs */
				//ws_handle_headers(_S(ws_context->header), _S(ws_context->field));
//
				//sbuf_clear(ws_context->header);
				//sbuf_clear(ws_context->field);
//
			//case HTTP_EVT_HEADER_DONE:
				///* fir off header now */
				//ws_acknowledge(ws);
		//}
	//} else if (ws->onmessage)
		///* We got an incomming message and a function to handle it */
		//ws->onmessage(ws, msg, nbytes);
//
	///* Websocket is to be kept alive until its EXPLICITLY closed */
	//return CONN_KEEP_ALIVE;
//}
//
//void
////ws_on_close(struct fd_context_t *context)
//{
	//printf(">>> on close!\n");
//}
//
///******************************************************************************/

void
ws_close(ws_t *ws)
{
	//close(ws->fd);
}

size_t
ws_send(ws_t *ws, const char *buf, size_t nbytes)
{
	char _buf[nbytes + 2];
	char *b = _buf;
	int i;

	*b++ = 0x00;
	for (i = 0; i < nbytes; ++i)
		*b++ = *buf++;
	*b = (char)0xff;

	return write(ws->base.fd, _buf, nbytes + 2);
}
