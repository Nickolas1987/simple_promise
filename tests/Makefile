CC=g++
CXXFLAGS=-c -pthread -std=c++1z -Wall
INC=  -I../src
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LD_LIBRARY_PATH = /usr/lib/x86_64-linux-gnu
EXECUTABLE=tests_simple_promise

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.28 -o $@ -pthread -lgtest -lgtest_main
.cpp.o:
	$(CC) $(CXXFLAGS) $< -o $@ $(INC)
clean:
	rm -rf *.o tests_simple_promise
