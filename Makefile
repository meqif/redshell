RAGEL = ragel
ECHO = echo
CC = gcc

PROG = redshell

SRC  = src/alias.c src/builtins.c src/command.c src/jobs.c \
       src/helper.c src/parser.c src/prompt.c src/queue.c src/hash-table.c \
       src/hash-string.c src/compare-string.c

SRCS = $(SRC) src/main.c
HDRS = $(SRC:.c=.h) src/common.h

OBJS = $(SRCS:.c=.o)

CFLAGS += -Os -Wall -g \
	  $(shell pkg-config --cflags glib-2.0 libedit)
LIBS += $(shell pkg-config --libs glib-2.0 libedit)

all: build/ $(PROG)

src/parser.c: src/parser.rl
	@${ECHO} RAGEL src/parser.rl
	@${RAGEL} src/parser.rl -C -o $@

$(PROG): $(OBJS)
	@${CC} -o $(PROG) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) dep.mak

dep.mak:	$(SRCS) $(HDRS)
	cc -MM $(SRCS) > dep.mak

include dep.mak
-include Tests.mk
