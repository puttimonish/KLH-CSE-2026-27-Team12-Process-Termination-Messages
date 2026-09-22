CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Isrc
LDFLAGS = -pthread

TARGET = process_termination

SRC = src/main.c \
      src/process/process_manager.c \
      src/process/signal_handler.c \
      src/communication/ipc.c \
      src/monitoring/memory_monitor.c \
      src/monitoring/thread_monitor.c \
      src/logging/logger.c \
      src/dashboard/dashboard.c \
      src/logging/event_store.c

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
