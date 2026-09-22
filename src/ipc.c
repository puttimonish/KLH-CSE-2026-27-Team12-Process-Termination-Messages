#include "ipc.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

int create_pipe(int pipe_fd[2])
{
    return pipe(pipe_fd);
}

int send_message(int fd, const char *message)
{
    if (message == NULL)
        return -1;

    size_t length = strlen(message) + 1;

    ssize_t written = write(fd, message, length);

    if (written < 0)
        return -1;

    return (written == (ssize_t)length) ? 0 : -1;
}

int receive_message(int fd, char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0)
        return -1;

    ssize_t bytes_read = read(fd, buffer, buffer_size - 1);

    if (bytes_read < 0)
        return -1;

    buffer[bytes_read] = '\0';

    return (int)bytes_read;
}
