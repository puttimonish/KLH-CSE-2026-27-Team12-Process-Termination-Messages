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
      src/monitoring/process_inspector.c \
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

# CO4-CO6 standalone experiments
MEMORY_EXPERIMENT = memory_experiment
IO_EXPERIMENT = file_io_experiment
THREAD_EXPERIMENT = thread_experiment

.PHONY: experiments

experiments: $(MEMORY_EXPERIMENT) $(IO_EXPERIMENT) $(THREAD_EXPERIMENT)

$(MEMORY_EXPERIMENT): src/experiments/memory_experiment.c
	$(CC) $(CFLAGS) $< -o $@

$(IO_EXPERIMENT): src/experiments/file_io_experiment.c
	$(CC) $(CFLAGS) $< -o $@

$(THREAD_EXPERIMENT): src/experiments/thread_experiment.c
	$(CC) $(CFLAGS) -pthread $< -o $@
