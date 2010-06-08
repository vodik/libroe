#ifndef SMALLHTTP_TEST_HTTP
#define SMALLHTTP_TEST_HTTP

#include <http.h>
#include <websocket.h>

void test_onrequest(http_t *conn, request_t *reqest, response_t *response);
void ws_onrequest(ws_t *ws);

#endif
