CC=gcc
CFLAGS=-mfloat-abi=hard -mfpu=vfpv3-d16 -march=armv7-a -mthumb -s
INCLUDES=-I./include
LFLAGS=
LIBS=
SRCS=gonzales.c fast-gpio.c
OBJS=$(SRCS:.c=.o)

MAIN=gonzales

.PHONY: depend clean

all:	$(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INcLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
