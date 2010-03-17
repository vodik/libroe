#ifndef SMALLHTTP_SMALLHTTP
#define SMALLHTTP_SMALLHTTP

typedef struct {
	void (*connection)();
} httpd;

int smallhttp_main(int argc, char *argv[], httpd *ops);

#endif
