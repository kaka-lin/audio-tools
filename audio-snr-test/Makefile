# Toolchain path
#TCPATH = aarch64-linux-gnu-
TCPATH ?=

# Kernel Headers path
KERNELHEADERS = /usr/include/

CC := ${TCPATH}gcc
CFLAGS = -Wall -g
LDFLAGS = -lm -ltinyalsa -lpthread

INCLUDES = -I$(KERNELHEADERS) -I/usr/local/include/

SOURCES1 = capws.c src/audio.c src/args.c
OBJECTS1 := $(SOURCES1:.c=.o)
EXEC1 = capws

SOURCES2 = snr.c src/wav.c src/array.c
OBJECTS2 := $(SOURCES2:.c=.o)
EXEC2 = snr

all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJECTS1) $(LDFLAGS)

$(EXEC2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJECTS2) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f $(OBJECTS1) $(EXEC1) $(OBJECTS2) $(EXEC2)

install:

.PHONY: clean alls
