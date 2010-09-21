#include "response.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

#include "hashtable.h"

struct response {
	unsigned code;
	char *message;
	struct hashtable *headers;
};

static int
itoa(int n, char s[], size_t len, int radix)
{
	int i, t, sign;
	int digits;

	if (radix > 36)
		return 0;

	digits = (int)(log(n) / log(radix)) + 1;
	if (digits > len)
		return 0;
	i = digits - 1;

	if ((sign = n) < 0)
		n = -n;

	do {
		t = n % radix;
		s[i--] = t + (t > 9 ? 'a' - 10 : '0');
	} while (n /= radix);

	if (sign < 0)
		s[i] = '-';

	s[digits] = '\0';
	return digits;
}

char *
def_message(int code)
{
#define CM(code, msg) case code: return strdup(msg)
	switch (code) {
		CM(100, "Continue");
		CM(101, "Switching Protocols");
		
		CM(200, "OK");
		CM(201, "Created");
		CM(202, "Accepted");
		CM(203, "Non-Authoritative Information");
		CM(204, "No Content");
		CM(205, "Reset Content");
		CM(206, "Partial Content");

		CM(300, "Multiple Choices");
		CM(301, "Moved Permanently");
		CM(302, "Found");
		CM(303, "See Other");
		CM(304, "Not Modified");
		CM(305, "Use Proxy");
		CM(307, "Termporary Redirect");

		CM(400, "Bad Request");
		CM(401, "Unauthorized");
		CM(402, "Payment Required");
		CM(403, "Forbidden");
		CM(404, "Not Found");
		CM(405, "Method Not Allowed");
		CM(406, "Not Acceptable");
		CM(407, "Proxy Authentication Required");
		CM(408, "Request Timeout");
		CM(409, "Conflict");
		CM(410, "Gone");
		CM(411, "Length Required");
		CM(412, "Precondition Failed");
		CM(413, "Request Entry Too Large");
		CM(414, "Request-URI Too Large");
		CM(415, "Unsupported Media Type");
		CM(416, "Request Range Not Satisfiable");
		CM(417, "Expectation Failed");
		CM(418, "Conflict");

		CM(500, "Internal Server Error");
		CM(501, "Not Implemented");
		CM(502, "Bad Gateway");
		CM(503, "Service Unavailable");
		CM(504, "Gateway Timeout");
		CM(505, "HTTP Version Not Supported");
	}
	return NULL;
#undef CM
}

struct response *
response_new(int code, const char *message)
{
	struct response *response = malloc(sizeof(struct response));
	response->headers = hashtable_new(23, NULL);
	response->code = code;
	response->message = !message ? def_message(code) : message;
	return response;
}

void
response_free(struct response *response)
{
}

void
response_header_set(struct response *response, const char *header, const char *field)
{
}

void
response_write(struct response *response, const char *data, size_t len)
{
}

void
response_send(struct response *response, struct conn *conn)
{
}

/////////////////////////////////////////////////////////////////////////////

void
respond_with_file(struct conn *conn, const char *filename)
{
	struct response *response;
	char buf[10];

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		response = response_new(404, NULL);
	else {
		response = response_new(200, NULL);

		size_t filesize = lseek(fd, 0, SEEK_END);

		char *map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
		itoa(filesize, buf, 10, 10);

		response_header_set(response, "Content-Type", "text/html");
		response_header_set(response, "Content-Length", buf);
		response_write(response, map, filesize);

		munmap(map, filesize);
		close(fd);
	}

	printf("--> sent\n");
	response_send(response, conn);
	response_free(response);
}
