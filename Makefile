PROG = redshell
SRCS = redshell.c alias.c builtins.c command.c jobs.c helper.c pipeline.c
HDRS = alias.h command.h common.h builtins.h jobs.h helper.h pipeline.h

OBJS = $(SRCS:.c=.o)

#CFLAGS += -g -W -Wall -Wshadow -Waggregate-return -Wstrict-prototypes
CFLAGS += -Os -Wall -g $(shell pkg-config --cflags glib-2.0)
LIBS += $(shell pkg-config --libs glib-2.0)

$(PROG): $(OBJS)
	cc -o $(PROG) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) dep.mak

dep.mak:	$(SRCS) $(HDRS)
	cc -MM $(SRCS) > dep.mak

include dep.mak
