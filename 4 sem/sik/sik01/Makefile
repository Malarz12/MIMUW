CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

.PHONY: all clean

TARGET1 = echo-client-modified
TARGET2 = echo-server-modified

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).o err.o
$(TARGET2): $(TARGET2).o err.o

# To są zależności wygenerowane automatycznie za pomocą polecenia `gcc -MM *.c`.
echo-client-modified.o: echo-client-modified.c err.h
echo-server-modified.o: echo-server-modified.c err.h
echo-client-modified.o: echo-client-modified.c err.h
echo-server-modified.o: echo-server-modified.c err.h
err.o: err.c err.h

clean:
	rm -f $(TARGET1) $(TARGET2) *.o *~
