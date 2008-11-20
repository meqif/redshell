PROG = redshell
SRCS = redshell.c builtins.c jobs.c helper.c
HDRS = common.h builtins.h jobs.h helper.h

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
