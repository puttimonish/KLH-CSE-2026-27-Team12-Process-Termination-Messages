#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_PATH "data/co5_io_demo.txt"

int main(void)
{
    const char message[] =
        "Process Termination Messages: CO5 file I/O experiment.\n";
    char buffer[256] = {0};

    printf("========================================\n");
    printf("       FILE SYSTEM AND I/O EXPERIMENT\n");
    printf("========================================\n");

    int fd = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("File opened successfully. FD: %d\n", fd);

    ssize_t written = write(fd, message, sizeof(message) - 1);

    if (written == -1) {
        perror("write");
        close(fd);
        return EXIT_FAILURE;
    }

    if ((size_t)written != sizeof(message) - 1) {
        fprintf(stderr, "Write was incomplete.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Bytes written: %zd\n", written);

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return EXIT_FAILURE;
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    printf("Bytes read: %zd\n", bytes_read);
    printf("Read content: %s", buffer);

    struct stat info;

    if (fstat(fd, &info) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("File size: %lld bytes\n", (long long)info.st_size);

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    printf("File descriptor closed successfully.\n");

    int missing_fd = open("data/file_that_does_not_exist.txt", O_RDONLY);

    if (missing_fd == -1) {
        printf("Expected error test: ");
        printf("opening a missing file failed: %s\n", strerror(errno));
    } else {
        printf("Unexpectedly opened the test file.\n");
        close(missing_fd);
    }

    fd = open(FILE_PATH, O_RDONLY);

    if (fd == -1) {
        perror("open for mmap");
        return EXIT_FAILURE;
    }

    if (fstat(fd, &info) == -1) {
        perror("fstat for mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    if (info.st_size == 0) {
        printf("Cannot map an empty file.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    void *mapped = mmap(NULL, (size_t)info.st_size,
                        PROT_READ, MAP_PRIVATE, fd, 0);

    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Mapped file content: %.*s",
           (int)info.st_size, (const char *)mapped);

    if (munmap(mapped, (size_t)info.st_size) == -1) {
        perror("munmap");
        close(fd);
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("close after mmap");
        return EXIT_FAILURE;
    }

    printf("Memory mapping released successfully.\n");
    printf("CO5 experiment completed.\n");

    return EXIT_SUCCESS;
}
