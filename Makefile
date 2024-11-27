CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O0
TARGET = ecc_memory_manager

SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run:
	./$(TARGET)

.PHONY: all clean run