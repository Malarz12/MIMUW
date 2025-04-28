CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

.PHONY: all clean

TARGETS = nk-client-udp6 nk-server-udp6 multi-recv multi-send multi-recv6 multi-send6

all: $(TARGETS)

nk-client-udp6: nk-client-udp6.o err.o common.o
nk-server-udp6: nk-server-udp6.o err.o common.o
multi-recv6: multi-recv6.o err.o common.o
multi-recv: multi-recv.o err.o common.o
multi-send6: multi-send6.o err.o common.o
multi-send: multi-send.o err.o common.o

# To są zależności wygenerowane automatycznie za pomocą polecenia `gcc -MM *.c`.
common.o: common.c err.h common.h
err.o: err.c err.h
multi-recv6.o: multi-recv6.c err.h common.h
multi-recv.o: multi-recv.c err.h common.h
multi-send6.o: multi-send6.c err.h common.h
multi-send.o: multi-send.c err.h common.h
nk-client-udp6.o: nk-client-udp6.c err.h common.h
nk-server-udp6.o: nk-server-udp6.c err.h common.h

clean:
	rm -f $(TARGETS) *.o *~
