RAGEL = ragel
ECHO = echo
CC = gcc

PROG = redshell
SRCS = src/main.c src/alias.c src/builtins.c src/command.c src/jobs.c \
       src/helper.c src/parser.c src/pipeline.c src/prompt.c src/queue.c
HDRS = src/alias.h src/command.h src/common.h src/builtins.h src/jobs.h \
       src/helper.h src/parser.h src/pipeline.h src/prompt.h src/queue.h

OBJS = $(SRCS:.c=.o)

#CFLAGS += -g -W -Wall -Wshadow -Waggregate-return -Wstrict-prototypes \
#	  $(shell pkg-config --cflags glib-2.0 libedit)
CFLAGS += -Os -Wall -g \
	  $(shell pkg-config --cflags glib-2.0 libedit)
LIBS += $(shell pkg-config --libs glib-2.0 libedit)

all: build/ $(PROG)

src/parser.c: src/parser.rl
	@${ECHO} RAGEL core/pn-scan.rl
	@${RAGEL} src/parser.rl -C -o $@

$(PROG): $(OBJS)
	@${CC} -o $(PROG) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) dep.mak

dep.mak:	$(SRCS) $(HDRS)
	cc -MM $(SRCS) > dep.mak

include dep.mak
-include Tests.mk
