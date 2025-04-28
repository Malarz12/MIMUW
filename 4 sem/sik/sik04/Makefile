CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

# Dodajemy nowy cel poll-server-count
TARGETS = client poll-server poll-server-count echo-client-timeout echo-server-nonblocking

.PHONY: all clean

all: $(TARGETS)

client: client.o err.o common.o
poll-server: poll-server.o err.o common.o
poll-server-count: poll-server-count.o err.o common.o
echo-client-timeout: echo-client-timeout.o err.o common.o
echo-server-nonblocking: echo-server-nonblocking.o err.o common.o

# Zależności (wygenerowane automatycznie przy pomocy gcc -MM *.c)
client.o: client.c err.h common.h
common.o: common.c err.h common.h
poll-server.o: poll-server.c err.h common.h
poll-server-count.o: poll-server-count.c err.h common.h
echo-client-timeout.o: echo-client-timeout.c err.h common.h
echo-server-nonblocking.o: echo-server-nonblocking.c err.h common.h
err.o: err.c err.h

clean:
	rm -f $(TARGETS) *.o
