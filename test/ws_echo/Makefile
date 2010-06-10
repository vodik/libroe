CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I../../include -O0 -g -D_GNU_SOURCE
LDFLAGS = -L../../lib -lsmallhttp -lm

SRC = ws_echo.c
OBJ = ${SRC:.c=.o}

all: ws_echo

ws_echo: ${OBJ}
	@${CC} ${LDFLAGS} -o _$@ ${OBJ}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

clean:
	@echo cleaning...
	@rm -f _ws_echo ${OBJ}

.PHONY: all clean
