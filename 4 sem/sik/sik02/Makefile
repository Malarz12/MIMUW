CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

.PHONY: all clean

TARGET1 = sum-client
TARGET2 = sum-server

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).o sum-common.o err.o common.o
$(TARGET2): $(TARGET2).o sum-common.o err.o common.o

# To są zależności wygenerowane automatycznie za pomocą polecenia `gcc -MM *.c`.

err.o: err.c err.h
common.o: common.c err.h common.h

sum-common.o: sum-common.c sum-common.h
sum-client.o: sum-client.c sum-common.h err.h common.h
sum-server.o: sum-server.c sum-common.h err.h common.h

clean:
	rm -f $(TARGET1) $(TARGET2) *.o *~
