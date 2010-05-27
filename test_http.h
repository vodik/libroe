#ifndef SMALLHTTP_TEST_HTTP
#define SMALLHTTP_TEST_HTTP

#include <services/http.h>
#include <services/websocks.h>

void test_onrequest(http_conn *conn);
void ws_onrequest(ws_t *ws);

#endif
