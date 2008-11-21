PROG = redshell
SRCS = redshell.c builtins.c command.c jobs.c helper.c pipeline.c
HDRS = command.h common.h builtins.h jobs.h helper.h pipeline.h

OBJS = $(SRCS:.c=.o)

#CFLAGS += -g -W -Wall -Wshadow -Waggregate-return -Wstrict-prototypes
CFLAGS += -Os -Wall -g


$(PROG): $(OBJS)
	cc -o $(PROG) $(OBJS)


clean:
	rm -f $(OBJS) dep.mak

dep.mak:	$(SRCS) $(HDRS)
	cc -MM $(SRCS) > dep.mak

include dep.mak
