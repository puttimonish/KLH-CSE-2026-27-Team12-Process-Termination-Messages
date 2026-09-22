#include "memory_monitor.h"

#include <stdio.h>
#include <string.h>

int memory_monitor_read(pid_t pid, MemoryInfo *info)
{
    if (!info)
        return -1;

    char path[128];

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return -1;

    memset(info, 0, sizeof(*info));

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        unsigned long value;

        if (sscanf(line, "VmSize: %lu kB", &value) == 1)
            info->virtual_memory_kb = value;

        else if (sscanf(line, "VmRSS: %lu kB", &value) == 1)
            info->resident_memory_kb = value;

        else if (sscanf(line, "RssShmem: %lu kB", &value) == 1)
            info->shared_memory_kb = value;

        else if (sscanf(line, "VmData: %lu kB", &value) == 1)
            info->data_memory_kb = value;
    }

    fclose(file);

    return 0;
}

void memory_monitor_print(pid_t pid, const MemoryInfo *info)
{
    if (!info)
        return;

    printf("\n");
    printf("============================================================\n");
    printf("                    MEMORY MONITOR\n");
    printf("============================================================\n");
    printf("Process PID        : %d\n", pid);
    printf("Virtual Memory     : %lu KB\n", info->virtual_memory_kb);
    printf("Resident Memory    : %lu KB\n", info->resident_memory_kb);
    printf("Shared Memory      : %lu KB\n", info->shared_memory_kb);
    printf("Data Memory        : %lu KB\n", info->data_memory_kb);
    printf("Memory Status      : MONITORED\n");
    printf("============================================================\n");
}
