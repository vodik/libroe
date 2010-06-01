#include "http.h"

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

#include <parser.h>
#include <util.h>

////////////////////////////////////////////////////////////////////////////////

/** 
* @brief Function responding to an incoming http connection. Creates a context.
* 
* @param context A structure to store the context in.
*/
void
http_on_open(conn_t *conn)
{
	printf("--> opening\n");
}

/** 
* @brief Function responding to incoming data from an http connection. Incoming
* data should make up an HTTP http, which we parse and upon completion, delegate
* further to http service specific callbacks in response to different HTTP methods
* such as GET or POST.
* 
* @param context A structure storing the context.
* @param msg The incoming data.
* @param nbytes The length of the incoming data. Max size specified in config.h in
* the macro POLL_MGMT_BUFF_SIZE.
* 
* @return A keep-alive state. The polling subsystem closes the connection if this is
* not set.
*/

int
http_on_message(conn_t *conn, const char *msg, size_t nbytes)
{
	http_parser reader;
	const char *buf;
	size_t len;
	int code;

	http_parser_init(&reader, conn);

	while ((code = http_parser_next(&reader, &buf, &len)) > 0) {
		/* use buf, len */
	}
	if (code < 0)
		die("error");
	
	http_parser_cleanup(&reader);

	/* send request */

	/* send headers */

	/* on body */
	http_response response;

	http_response_init(&response, conn);

	/* send */

	http_response_cleanup(&response);
}

/** 
* @brief Function responding to a connection closing. Currently a nop.
* NOTE: do not use this function to cleanup context data. A destructor is
* already specified elsewhere.
* 
* @param context A structure storing the context.
*/
void
http_on_close(conn_t *conn)
{
}
