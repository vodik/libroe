#include "test_http.h"

void static_serve(http_conn *conn)
{
	int fd, filesize;
	char *map;

	printf("--> sending file\n");
	fd = open(path + 1, O_RDONLY); /* paths start with / */
	if (fd == -1) {
		http_response_error(response, 404, "Not Found");
		return;
	}

	filesize = lseek(fd, 0, SEEK_END);
	map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);

	http_response_begin(response, TRANSFER_ENCODING_NONE, 200, "OK", mime, filesize);
	http_response_write(response, map, filesize);
	http_response_end(response);
	printf("--> sent\n");

	munmap(map, filesize);
	close(fd);

	conn->keep_alive = 0;
}

void logger(http_conn *conn, const char *header, const char *field)
{
	if (strcmp(header, "User-Agent") == 0) {
		FILE *fp = fopen("useragents");
		fprintf(fg, "got: %s\n", field);
		close(fp);
	}
}

void test_onrequest(http_conn *conn)
{
	if (!conn->request.method != HTTP_METHOD_GET) {
		fprintf("only GET supported at the moment");
		conn->keep_alive = 0;
	}

	conn->onheaders = logger;
	conn->makeresponse = static_server;
}
