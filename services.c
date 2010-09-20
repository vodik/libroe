#include "services.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "websockets.h"
#include "util.h"
#include "io.h"

typedef IO *(*ioinit)(int port);

struct service {
	char *name;
	int port;
	IO *io;
	int active;
	roe_cb cb;
	const struct service_descrpt *descrpt;
	struct service *next;
};

struct service_descrpt {
	const char *name;
	ioinit init;
	iofunc func;
};

static const struct service_descrpt Services[] = {
	{ "http",      &http_init, &http_accept },
	{ "websocket", &ws_init,   &ws_iofunc   },
};

static struct service *root = NULL;

static const struct service_descrpt *
find_service(const char *name)
{
	int i;
	for (i = 0; i < LENGTH(Services); ++i) {
		if (strcmp(name, Services[i].name) == 0)
			return &Services[i];
	}
	return NULL;
}

struct service *
roe_new(const char *name, int port)
{
	const struct service_descrpt *descrpt = find_service(name);
	if (descrpt) {
		struct service *service = malloc(sizeof(struct service));

		service->port = port;
		service->descrpt = descrpt;
		service->active = 0;
		service->next = root;
		root = service;

		return service;
	}
	return NULL;
}

void
roe_start(struct service *service, roe_cb cb)
{
	printf("--> starting %s service on %d\n", service->descrpt->name, service->port);

	service->active = 1;
	service->cb = cb;
	service->io = service->descrpt->init(service->port);
	io_watch(service->io, IO_IN | IO_HUP, service->descrpt->func, service);
}

void
roe_stop(struct service *service)
{
	io_close(service->io);
	free(service);
}
