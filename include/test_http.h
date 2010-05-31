#ifndef SMALLHTTP_TEST_HTTP
#define SMALLHTTP_TEST_HTTP

#include <http.h>
#include <websocks.h>

void test_onrequest(http_conn *conn);
void ws_onrequest(ws_t *ws);

#endif
