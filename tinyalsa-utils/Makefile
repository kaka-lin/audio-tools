# Toolchain path
#TCPATH = aarch64-linux-gnu-
TCPATH ?=

# Kernel Headers path
KERNELHEADERS = /usr/include/

CC := ${TCPATH}gcc
CFLAGS = -Wall -g -lm
LDFLAGS = -ltinyalsa -lpthread

INCLUDES = -I$(KERNELHEADERS) -I/usr/local/include/

#SRCDIR = src
#SOURCES = $(wildcard $(SRCDIR)/*.c)
SOURCES1 = tinycap.c src/audio.c src/args.c
OBJECTS1 := $(SOURCES1:.c=.o)
EXEC1 = tinycap

SOURCES2 = tinyplay.c src/audio.c src/args.c
OBJECTS2 := $(SOURCES2:.c=.o)
EXEC2 = tinyplay

SOURCES3 = tinycaplay.c src/audio.c src/args.c
OBJECTS3 := $(SOURCES3:.c=.o)
EXEC3 = tinycaplay

all: $(EXEC1) $(EXEC2) $(EXEC3)

$(EXEC1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJECTS1) $(LDFLAGS)

$(EXEC2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJECTS2) $(LDFLAGS)

$(EXEC3): $(OBJECTS3)
	$(CC) $(CFLAGS) -o $(EXEC3) $(OBJECTS3) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f $(OBJECTS1) $(EXEC1) $(OBJECTS2) $(EXEC2) $(OBJECTS3) $(EXEC3)

install:

.PHONY: clean alls
