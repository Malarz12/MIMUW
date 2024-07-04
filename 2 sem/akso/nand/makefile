# Compiler
CC=gcc

# Compiler flags
CFLAGS=-Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2

# Linker flags
LDFLAGS=-shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

# Source files
SOURCES=nand.c memory_tests.c

# Object files
OBJECTS=$(SOURCES:.c=.o)

# Header files
HEADERS=nand.h memory_tests.h

# Library name
LIBRARY=libnand.so

# Example application name
EXAMPLE=nand_example

# Phony targets
.PHONY: all clean

# Default target: compile the library and the example application
all: $(LIBRARY) $(EXAMPLE)

# Rule for compiling the library
$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule for compiling the example application
$(EXAMPLE): nand_example.o $(LIBRARY)
	$(CC) -o $@ $< -L. -lnand

# Rule for compiling C source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean target: remove object files, library, and example application
clean:
	rm -f $(OBJECTS) $(LIBRARY) $(EXAMPLE) nand_example.o