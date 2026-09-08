CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = scheduler
SRCS = main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.out