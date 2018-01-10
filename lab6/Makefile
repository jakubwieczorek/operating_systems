BUILD=g++
LINK=g++
OBJECTS=main.o
TARGET=manager.out
FLAGS=-g -Wall -std=c++11 -c

$(TARGET): $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS)

main.o: main.cpp main.h
	$(BUILD) $(FLAGS) -o main.o main.cpp

clean:
	rm -rf *.o 
