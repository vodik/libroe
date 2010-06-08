#ifndef SMALLHTTP_TEST_HTTP
#define SMALLHTTP_TEST_HTTP

#include <http.h>
#include <websocket.h>

void test_onrequest(http_t *conn, request_t *reqest, response_t *response);
void ws_onopen(ws_t *ws, request_t *request);

#endif
