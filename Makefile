CC=g++-4.9
CFLAGS=-c -std=c++14 -Wall
LDFLAGS=-lpthread
SOURCES=GPIOtestWithThreadDriver.cpp GPIOClass.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=GPIOtestWithThreadDriver

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: *.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
