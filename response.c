#include "response.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

struct response {
	int res;
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

struct response *
response_new(int code, const char *message)
{
	struct response *response = malloc(sizeof(struct response));
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
