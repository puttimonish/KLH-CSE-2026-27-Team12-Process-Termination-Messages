#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

#define MEMORY_MB 16

static void print_usage(const char *label,
                        const struct rusage *usage)
{
    printf("\n%s\n", label);
    printf("Minor page faults             : %ld\n",
           usage->ru_minflt);
    printf("Major page faults             : %ld\n",
           usage->ru_majflt);
    printf("Voluntary context switches    : %ld\n",
           usage->ru_nvcsw);
    printf("Involuntary context switches  : %ld\n",
           usage->ru_nivcsw);
}

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0) {
        fprintf(stderr, "Unable to determine page size.\n");
        return EXIT_FAILURE;
    }

    size_t bytes = (size_t)MEMORY_MB * 1024 * 1024;
    struct rusage before, after;

    printf("========================================\n");
    printf("      MEMORY MANAGEMENT EXPERIMENT\n");
    printf("========================================\n");
    printf("PID         : %ld\n", (long)getpid());
    printf("Page size   : %ld bytes\n", page_size);
    printf("Allocation  : %d MiB\n", MEMORY_MB);

    if (getrusage(RUSAGE_SELF, &before) == -1) {
        perror("getrusage");
        return EXIT_FAILURE;
    }

    unsigned char *memory = malloc(bytes);

    if (memory == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    printf("\nAllocated memory successfully.\n");
    printf("Touching one byte per memory page...\n");

    for (size_t offset = 0; offset < bytes;
         offset += (size_t)page_size) {
        memory[offset] = 1;
    }

    memory[bytes - 1] = 1;

    if (getrusage(RUSAGE_SELF, &after) == -1) {
        perror("getrusage");
        free(memory);
        return EXIT_FAILURE;
    }

    print_usage("BEFORE TOUCHING ALLOCATED MEMORY", &before);
    print_usage("AFTER TOUCHING ALLOCATED MEMORY", &after);

    printf("\nChanges observed during the experiment:\n");
    printf("Minor page faults : +%ld\n",
           after.ru_minflt - before.ru_minflt);
    printf("Major page faults : +%ld\n",
           after.ru_majflt - before.ru_majflt);

    free(memory);
    printf("\nMemory released successfully.\n");

    return EXIT_SUCCESS;
}
