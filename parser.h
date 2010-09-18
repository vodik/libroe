#ifndef LIBROE_REQUEST_PARSER
#define LIBROE_REQUEST_PARSER

#include "io.h"
#include "string.h"
#include "hashtable.h"

struct request {
	struct string *method;
	struct string *path;
	struct string *version;
	struct hashtable *headers;
};

struct request *parse_request(IO *io);

#endif
