#ifndef LIBROE_REQUEST_PARSER
#define LIBROE_REQUEST_PARSER

#include "io.h"
#include "string.h"
#include "hashtable.h"

struct request {
	char *method;
	char *path;
	char *version;
	struct hashtable *headers;
};

struct request *request_new();

void parse_request(struct request *request, IO *io);
void request_free(struct request *request);

const char *request_header(struct request *request, const char *header);


#endif
