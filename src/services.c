#include <services.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <http.h>
#include <websocket.h>
#include <irc.h>
#include <util.h>

typedef struct {
	const char *name;
	const fd_cbs_t *cbs;
} srv_descpt_t;

typedef struct _srv_list srv_list_t;
struct _srv_list {
	service_t service;
	srv_list_t *next;
};

static const srv_descpt_t Services[] = {
	{ "http",      &http_callbacks },
	{ "websocket", &ws_callbacks   },
	{ "irc",       &irc_callbacks  },
};

const srv_descpt_t *
find_service(const char *name)
{
	int i;
	for (i = 0; i < LENGTH(Services); ++i) {
		if (strcmp(name, Services[i].name) == 0)
			return &Services[i];
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

int
pull_request(request_t *request, parser_t *parser)
{
	int code;
	const char *b;
	size_t len;
	static char *header;

	while ((code = parser_next(parser, &b, &len)) > 0) {
		switch (code) {
			case HTTP_DATA_METHOD:
				printf("==> METHOD: \"%s\"\n", b);
				request->method = strndup(b, len);
				break;
			case HTTP_DATA_PATH:
				printf("==> PATH: \"%s\"\n", b);
				request->path = strndup(b, len);
				break;
			case HTTP_DATA_VERSION:
				printf("==> VERSION: \"%s\"\n", b);
				request->version = strndup(b, len);
				break;
			case HTTP_DATA_HEADER:
				printf("==> HEADER: \"%s\"\n", b);
				header = strndup(b, len);
				break;
			case HTTP_DATA_FIELD:
				printf("==> FIELD: \"%s\"\n", b);
				hashtable_add(&request->headers, header, strndup(b, len));
				free(header);
				break;
		}
	}
	return code;
}

////////////////////////////////////////////////////////////////////////////////

static srv_list_t *root = NULL;

int
roe_start(const char *name, poll_mgmt_t *mgmt, int port, const void *iface)
{
	const srv_descpt_t *srv_desc = find_service(name);
	if (srv_desc) {
		srv_list_t *srv = malloc(sizeof(srv_list_t));
		srv->service.fd = poll_mgmt_listen(mgmt, port, srv_desc->cbs, iface);

		printf("--> starting %s@%d - fd:%d\n", name, port, srv->service.fd);

		srv->next = root;
		root = srv;
		return 0;
	}
	return -1;
}

void
roe_stop()
{
	srv_list_t *srv = root, *next;
	while (srv) {
		next = srv->next;
		free(srv);
		srv = next;
	}
	root = NULL;
}
