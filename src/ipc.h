#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

int create_pipe(int pipe_fd[2]);
int send_message(int fd, const char *message);
int receive_message(int fd, char *buffer, int buffer_size);

#endif
