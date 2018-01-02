BUILD=gcc
LINK=gcc
OBJECTS=main.o queue.o
TARGET=factory
FLAGS=-g -Wall -std=c99 -c

$(TARGET): $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS)
main.o: main.c main.h
	$(BUILD) $(FLAGS) -o main.o main.c

clean:
	rm -rf *.o 
