CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -pthread

TARGET = process_termination

SRC = src/main.c \
      src/process_manager.c \
      src/signal_handler.c \
      src/ipc.c \
      src/memory_monitor.c \
      src/thread_monitor.c \
      src/logger.c \
      src/dashboard.c \
      src/event_store.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
