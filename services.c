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
	IO *io;
	struct service *next;
};

struct service_descrpt {
	const char *name;
	ioinit init;
	iofunc func;
};

static const struct service_descrpt Services[] = {
	{ "http",      &http_init, &http_iofunc },
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
roe_start(const char *name, int port)
{
	const struct service_descrpt *descrpt = find_service(name);
	if (descrpt) {
		struct service *service = malloc(sizeof(struct service));

		printf("--> starting %s service on %d\n", name, port);

		service->io = descrpt->init(port);
		io_watch(service->io, IO_IN | IO_HUP, descrpt->func, NULL);
		service->next = root;
		root = service;
		return service;
	}
	return NULL;
}
