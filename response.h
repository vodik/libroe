#ifndef LIBROE_RESPONSE_H
#define LIBROE_RESPONSE_H

#include "conn.h"

struct response;

struct response *response_new(int code, const char *message);
void response_free(struct response *response);

void response_header_set(struct response *response, const char *header, const char *field);
void response_write(struct response *response, const char *data, size_t len);
void response_send(struct response *response, struct conn *conn);

#endif
