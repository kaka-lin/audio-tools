# Toolchain path
#TCPATH = aarch64-linux-gnu-
TCPATH ?=

# Kernel Headers path
KERNELHEADERS = /usr/include/

CC := ${TCPATH}gcc
CFLAGS = -Wall -g 
LDFLAGS = -lasound -lm

INCLUDES = -I$(KERNELHEADERS)

SRCDIR = src
BUILDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:.c=.o)
EXEC = alsa-example

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f $(OBJECTS) $(EXEC)

install:

.PHONY: clean alls
