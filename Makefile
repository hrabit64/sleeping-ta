CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = sleeping_ta
SRCS = main.c seat_queue.c student.c ta.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)