.POSIX:
.SUFFIXES:
.SUFFIXES: .c

CC = c99
CFLAGS = -Ofast
FLAGS_GSL = `pkg-config --cflags --libs gsl`
M = \
mueler-brown\

all: $M
.c:; $(CC) $< -o $@ $(CFLAGS) $(LDFLAGS) $(FLAGS_GSL) -lm
clean:; rm -f -- $M
