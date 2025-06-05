# Makefile for approx-server and approx-client
CXX       := g++
CXXFLAGS  := -std=c++20 -Wextra -Wpedantic -Wshadow \
             -Wold-style-cast -Wnon-virtual-dtor -Wnull-dereference -DDEBUG

SERVER_EXE := approx-server
CLIENT_EXE := approx-client

# Źródła tylko te dwa pliki .cpp:
SRCS := approx-server.cpp approx-client.cpp
OBJS := $(SRCS:.cpp=.o)

HDRS := common.hpp message.hpp player.hpp err.h

.PHONY: all clean

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): approx-server.o           # err.o usunięty
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CLIENT_EXE): approx-client.o           # err.o usunięty
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(SERVER_EXE) $(CLIENT_EXE)
