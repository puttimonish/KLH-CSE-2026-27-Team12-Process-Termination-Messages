#include "communication/ipc.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int ipc_create(int pipe_fd[2])
{
    if (pipe_fd == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    return pipe(pipe_fd);
}

int ipc_send_message(int fd, const char *message)
{
    if (fd < 0 || message == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    size_t length = strlen(message);
    size_t total_written = 0;

    while (total_written < length)
    {
        ssize_t written = write(
            fd,
            message + total_written,
            length - total_written
        );

        if (written < 0)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (written == 0)
        {
            errno = EIO;
            return -1;
        }

        total_written += (size_t)written;
    }

    return 0;
}

int ipc_receive_message(int fd, char *buffer, size_t buffer_size)
{
    if (fd < 0 || buffer == NULL || buffer_size == 0)
    {
        errno = EINVAL;
        return -1;
    }

    ssize_t bytes_read;

    do
    {
        bytes_read = read(fd, buffer, buffer_size - 1);
    }
    while (bytes_read < 0 && errno == EINTR);

    if (bytes_read < 0)
        return -1;

    buffer[bytes_read] = '\0';

    return (int)bytes_read;
}

void ipc_close(int fd)
{
    if (fd >= 0)
        (void)close(fd);
}
