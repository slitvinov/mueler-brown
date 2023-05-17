.POSIX:
.SUFFIXES:
.SUFFIXES: .c

CC = c99
M = \
main\

all: $M
.c:; $(CC) $< -o $@ $(CFLAGS) $(LDFLAGS) -lm
clean:; rm -f -- $M
