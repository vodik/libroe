#include <services.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <http.h>
#include <websocket.h>
/*#include <irc.h>*/
#include <util.h>

typedef struct {
	const char *name;
	const fd_cbs_t *cbs;
} srv_descpt_t;

static const srv_descpt_t Services[] = {
	{ "http",      &http_callbacks },
	{ "websocket", &ws_callbacks   },
	/*{ "irc",       &irc_callbacks  },*/
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
service_start(service_t *service, const char *name, poll_mgmt_t *mgmt, int port, void *iface)
{
	const srv_descpt_t *srv_desc = find_service(name);
	if (srv_desc) {
		service->fd = poll_mgmt_listen(mgmt, port, srv_desc->cbs, iface);
		printf("--> starting %s@%d - fd:%d\n", name, port, service->fd);
		return 0;
	} else {
		die("failed to match server \"%s\"\n", name);
		return 1;
	}
}
