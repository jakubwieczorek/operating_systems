BUILD=gcc
LINK=gcc
OBJECTS=main.o
TARGET=factory
FLAGS=-g -Wall -std=c99 -c

$(TARGET): $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS)
main.o:
	$(BUILD) $(FLAGS) -o main.o main.c

clean:
	rm -rf *.o 
