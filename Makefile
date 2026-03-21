CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = his_demo
SRC = his_demo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

export: $(TARGET)
	printf '16\n0\n' | ./$(TARGET)

clean:
	rm -f $(TARGET) *.o
