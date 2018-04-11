\CC=g++
CXXFLAGS=-c -pthread -std=c++1y -Wall
INC=  -I../src
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LD_LIBRARY_PATH = /usr/lib/x86_64-linux-gnu
EXECUTABLE=tests_simple_promise

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.24 -o $@ -pthread -lgmock -lgmock_main
.cpp.o:
	$(CC) $(CXXFLAGS) $< -o $@ $(INC)
clean:
	rm -rf *.o tests_simple_promise
