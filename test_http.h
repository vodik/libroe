#ifndef SMALLHTTP_TEST_HTTP
#define SMALLHTTP_TEST_HTTP

#include <services/http.h>

void test_onrequest(http_conn *conn, request_data *data);

static http_ops {
	.port = 33456;
	.onrequest = test_onrequest;
}

#endif
