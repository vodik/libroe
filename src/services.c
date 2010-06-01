#include <services.h>

#include <http.h>
#include <websocket.h>
#include <irc.h>

static const srv_descpt_t Services[] = {
	{ "http",      &http_events_t },
	{ "websocket", &ws_events_t   },
	{ "irc",       &irc_events_t  },
};

const srv_descpt_t *
find_service(const char *name)
{
	int i;
	for (i = 0; i < LENGTH(Services); ++i) {
		if (strcmp(c, Services[i].name) == 0)
			return &Services[i];
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

int
service_start(service_t *service, const char *name, poll_mgmt_t *mgmt, int port, void *iface)
{
	const srv_descript_t *srv_desc = find_service(name);
	service->fd = poll_mgmt_listen(mgmt, srv_desc->cbs, iface);
}
