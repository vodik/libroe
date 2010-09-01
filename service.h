#ifndef LIBROE2_SERVICES
#define LIBROE2_SERVICES

struct service;

struct service *roe_start(const char *name, int port);

#endif
