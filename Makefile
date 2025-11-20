CC := clang
CFLAGS := -g -Wall -fsanitize=address

all: alliance

clean:
	rm -rf alliance alliance.dSYM

alliance: main.c map.c map.h util.c util.h
	$(CC) $(CFLAGS) -o alliance main.c map.c util.c -lncurses

.PHONY: all clean
