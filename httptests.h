#ifndef SMALLHTTP_HTTPTESTS
#define SMALLHTTP_HTTPTESTS

#include "httpparser.h"

#define TEST_ELEMENT_SIZE 2048
#define TEST_MAX_HEADERS 10

typedef struct request_testdata request_testdata;

struct request_testdata {
	const char *name;
	const char *raw;
	int keep_alive;
	int method;
	char path[TEST_ELEMENT_SIZE];
	char query[TEST_ELEMENT_SIZE];
	char fragment[TEST_ELEMENT_SIZE];
	int headers;
	char fields[TEST_MAX_HEADERS][TEST_ELEMENT_SIZE];
	char values[TEST_MAX_HEADERS][TEST_ELEMENT_SIZE];
	char body[TEST_ELEMENT_SIZE];
};

static const struct request_testdata curl_get = {
	.name = "curl get test",
	.raw = "GET /test HTTP/1.1\r\n"
	       "User-Agent: curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n"
	       "Host: 0.0.0.0:5000\r\n"
	       "Accept: */*\r\n"
	       "\r\n",
	.keep_alive = 1,
	.method = HTTP_GET,
	.path = "/test",
	.query = "",
	.fragment = "",
	.headers = 3,
	.fields = {
		"User-Agent",
		"Host",
		"Accept"
	},
	.values = {
		"curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1"
		"0.0.0.0:5000",
		"*/*",
	},
	.body = "",
};

static const struct request_testdata firefox_get = {
	.name = "firefox get test",
	.raw = "GET /favicon.ico HTTP/1.1\r\n"
	       "Host: 0.0.0.0:5000\r\n"
	       "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
	       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	       "Accept-Language: en-us,en;q=0.5\r\n"
	       "Accept-Encoding: gzip,deflate\r\n"
	       "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
	       "Keep-Alive: 300\r\n"
	       "Connection: keep-alive\r\n"
	       "\r\n",
	.keep_alive = 1,
	.method = HTTP_GET,
	.path = "/favicon.icon",
	.query = "",
	.fragment = "",
	.headers = 8,
	.fields = {
		"Host",
		"User-Agent",
		"Accept",
		"Accept-Language",
		"Accept-Encoding",
		"Accept-Charset",
		"Keep-Alive",
		"Connection",
	},
	.values = {
		"0.0.0.0:5000",
		"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0",
		"text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
		"en-us,en;q=0.5",
		"gzip,deflate",
		"ISO-8859-1,utf-8;q=0.7,*;q=0.7",
		"300",
		"keep-alive",
	},
	.body = "",
};


static const struct request_testdata *fixtures[] = {
	&curl_get,
	&firefox_get,
};

#endif
