TARGET=main.out
OBJECTS=main.o
LINK=g++
BUILD=g++
FLAGS=-g -Wall -std=c++11 -c
LIBS=-lpthread

$(TARGET): $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS) $(LIBS)
main.o: main.cpp monitor.h
	$(BUILD) $(FLAGS) -o main.o main.cpp

clean:
	rm -rf *.o
