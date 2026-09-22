#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <sys/types.h>

typedef struct
{
    unsigned long virtual_memory_kb;
    unsigned long resident_memory_kb;
    unsigned long shared_memory_kb;
    unsigned long data_memory_kb;
} MemoryInfo;

int memory_monitor_read(pid_t pid, MemoryInfo *info);

void memory_monitor_print(pid_t pid, const MemoryInfo *info);

#endif
