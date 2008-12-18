PROG = redshell
SRCS = src/main.c src/alias.c src/builtins.c src/command.c src/jobs.c \
       src/helper.c src/parser.c src/pipeline.c
HDRS = src/alias.h src/command.h src/common.h src/builtins.h src/jobs.h \
       src/helper.h src/parser.h src/pipeline.h

OBJS = $(SRCS:.c=.o)

#CFLAGS += -g -W -Wall -Wshadow -Waggregate-return -Wstrict-prototypes \
#	  $(shell pkg-config --cflags glib-2.0)
CFLAGS += -Os -Wall -g \
	  $(shell pkg-config --cflags glib-2.0 libedit)
LIBS += $(shell pkg-config --libs glib-2.0 libedit)

all: build/ $(PROG)

$(PROG): $(OBJS)
	cc -o $(PROG) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) dep.mak

dep.mak:	$(SRCS) $(HDRS)
	cc -MM $(SRCS) > dep.mak

include dep.mak
-include Tests.mk
