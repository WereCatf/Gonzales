CC=gcc
CFLAGS=-mfloat-abi=hard -mfpu=vfpv3-d16 -march=armv7-a -mthumb -s -O2
INCLUDES=-I./include
LFLAGS=
LIBS=
MAINSRCS=gonzales.c fast-gpio.c
BENCHSRCS=bench.c fast-gpio.c
MAINOBJS=$(MAINSRCS:.c=.o)
BENCHOBJS=$(BENCHSRCS:.c=.o)
LSOC=
DLSOC=
USELIBSOC=0

ifeq ($(USELIBSOC), 1)
	LSOC=-lsoc
	DLSOC=-D_WITH_LIBSOC
endif

MAIN=gonzales
BENCH=bench

.PHONY: depend clean

all:	$(MAIN) $(BENCH)

$(MAIN): $(MAINOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(MAINOBJS) $(LFLAGS) $(LIBS)

$(BENCH): $(BENCHOBJS)
	$(CC) $(LSOC) $(CFLAGS) $(INCLUDES) -o $(BENCH) $(BENCHOBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(DLSOC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN) $(BENCH)

depend: $(MAINSRCS) $(BENCHSRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
