CC = clang
CFLAGS = -Wall -pedantic --std=c99
LDFLAGS =

SRC = test.c smallhttp.c server.c util.c
OBJ = ${SRC:.c=.o}

all: smallhttp_test

.c.o:
	@echo CC $@
	@${CC} -c ${CFLAGS} $<

smallhttp_test: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -f carbon ${OBJ}
