# The compiler command to use for C++ sources.
CXX      = g++

# Compilation flags: using C++20 standard and a variety of warnings enabled.
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic \
            -Wshadow -Wconversion -Wsign-conversion \
            -Wold-style-cast -Wnon-virtual-dtor \
            -Wnull-dereference -DDEBUG

# Executable targets for the main synchronization program and helper utilities.
TARGET        = peer-time-sync
SEND_LEADER   = send-leader
SEND_TIME     = send-time

# Source files for each target.
SRC           = peer-time-sync.cpp err.cpp message.cpp list.cpp
SEND_SRC      = send-leader.cpp err.cpp message.cpp list.cpp
SEND_TIME_SRC = send-time.cpp err.cpp message.cpp list.cpp

# Object files are derived from the source files by replacing .cpp with .o
OBJ           = $(SRC:.cpp=.o)
SEND_OBJ      = $(SEND_SRC:.cpp=.o)
SEND_TIME_OBJ = $(SEND_TIME_SRC:.cpp=.o)

# Header files that affect compilation order and dependency tracking.
HEADERS       = message.h list.h err.h helpers.h

# Mark the special 'all' and 'clean' targets as phony to avoid collisions
.PHONY: all clean

# Default rule: build all executables.
all: $(TARGET) $(SEND_LEADER) $(SEND_TIME)

# Link the peer-time-sync executable from its object files.
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# Link the send-leader utility from its object files.
$(SEND_LEADER): $(SEND_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(SEND_OBJ)

# Link the send-time utility from its object files.
$(SEND_TIME): $(SEND_TIME_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(SEND_TIME_OBJ)

# Compile .cpp files into .o object files, tracking all headers as prerequisites.
# This rule applies to all source files listed in SRC, SEND_SRC, and SEND_TIME_SRC.
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Special rule for err.cpp to ensure it compiles when err.h is updated.
err.o: err.cpp err.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target removes all generated binaries and object files.
clean:
	rm -f $(TARGET) $(SEND_LEADER) $(SEND_TIME) $(OBJ) $(SEND_OBJ) $(SEND_TIME_OBJ)
