CC=gcc
CFLAGS=-std=gnu99 -mfloat-abi=hard -mfpu=vfpv3-d16 -march=armv7-a -mthumb -s -Os
INCLUDES=-I./include
LFLAGS=
LIBS=
MAINSRCS=gonzales.c fast-gpio.c
BENCHSRCS=bench.c fast-gpio.c
DHTSRCS=dhtsensor.c fast-gpio.c
MAINOBJS=$(MAINSRCS:.c=.o)
BENCHOBJS=$(BENCHSRCS:.c=.o)
DHTOBJS=$(DHTSRCS:.c=.o)
LSOC=
DLSOC=
USELIBSOC=0

ifeq ($(USELIBSOC), 1)
	LSOC=-lsoc
	DLSOC=-D_WITH_LIBSOC
endif

MAIN=gonzales
BENCH=bench
DHT=dhtsensor

.PHONY: depend clean

all:	$(MAIN) $(BENCH) $(DHT)

$(MAIN): $(MAINOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(MAINOBJS) $(LFLAGS) $(LIBS)

$(BENCH): $(BENCHOBJS)
	$(CC) $(LSOC) $(CFLAGS) $(INCLUDES) -o $(BENCH) $(BENCHOBJS) $(LFLAGS) $(LIBS)

$(DHT): $(DHTOBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(DHT) $(DHTOBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(DLSOC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN) $(BENCH)

depend: $(MAINSRCS) $(BENCHSRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
