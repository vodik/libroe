CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I. -g
LDFLAGS =

SRC = main.c socks.c services/services.c services/http.c services/websocks.c request/parser.c request/request.c response/response.c util/hashtable.c util/urlencode.c util.c
OBJ = ${SRC:.c=.o}

all: test

.c.o:
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

test: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -f test ${OBJ}
