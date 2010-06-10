#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <smallhttp.h>
#include <util.h>

struct client {
	ws_t *client;
	struct client *next;
};

struct client *root = 0;
int connected = 0;

////////////////////////////////////////////////////////////////////////////////

void
chat_request(http_t *conn, request_t *request, response_t *response)
{
	int fd, filesize;
	char *map;
	char buf[10];

	fd = open("ws.html", O_RDONLY); /* paths start with / */
	if (fd == -1) {
		response_header_set(response, 404, NULL);
		return;
	}
	else
		response_header_set(response, 200, NULL);

	filesize = lseek(fd, 0, SEEK_END);
	map = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
	itoa(filesize, buf, 10, 10);

	response_header_add(response, "Content-Type", "text/html");
	response_header_add(response, "Content-Length", buf);
	response_write(response, map, filesize);
	printf("--> sent\n");

	munmap(map, filesize);
	close(fd);
}

void
chat_broadcast(ws_t *ws, const char *msg, size_t len)
{
	struct client *c = root;
	while (c) {
		conn_write(&c->client->base, msg, len);
		c = c->next;
	}
}

void
chat_ws_open(ws_t *ws, request_t *request)
{
	++connected;
	char msg[20];
	sprintf(msg, "Hello client %d", connected);
	conn_write(&ws->base, msg, strlen(msg));

	ws->onmessage = chat_broadcast;

	struct client *c;
	c = malloc(sizeof (struct client));
	c->client = ws;
	c->next = root;
	root = c;
}

////////////////////////////////////////////////////////////////////////////////

static const http_iface_t http_iface = {
	.onrequest = chat_request,
};

static const ws_iface_t ws_iface = {
	.onopen = chat_ws_open,
};

int
main(int argc, char *argv[])
{
	int hport = 11234, wsport = 33456;
	char c;

	while ((c = getopt(argc, argv, "p:w:")) != -1)
	{
		switch (c) {
			case 'p':
				hport = atoi(optarg);
				break;
			case 'w':
				wsport = atoi(optarg);
				break;
		}
	}

	poll_mgmt_t polling;
	service_t services[2];

	poll_mgmt_start(&polling, 10);
	service_start(&services[0], "http", &polling, hport, &http_iface);
	service_start(&services[1], "websocket", &polling, wsport, &ws_iface);

	int running = 0;
	while(running == 0) {
		running = poll_mgmt_poll(&polling, -1);
	}
	poll_mgmt_stop(&polling);
	return 0;
}
