#ifndef IPC_H
#define IPC_H

#include <stddef.h>

int ipc_create(int pipe_fd[2]);

int ipc_send_message(int fd, const char *message);

int ipc_receive_message(int fd, char *buffer, size_t buffer_size);

void ipc_close(int fd);

#endif
