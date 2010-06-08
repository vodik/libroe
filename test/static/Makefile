CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I../../include -O0 -g -D_GNU_SOURCE
LDFLAGS = -L../../lib -lsmallhttp -lm

SRC = main.c    \
	  static.c
OBJ = ${SRC:.c=.o}

all: static

static: ${OBJ}
	@${CC} ${LDFLAGS} -o _$@ ${OBJ}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

clean:
	@echo cleaning...
	@rm -f _static ${OBJ}

.PHONY: all clean
