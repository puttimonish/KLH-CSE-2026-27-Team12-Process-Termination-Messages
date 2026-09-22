#ifndef THREAD_MONITOR_H
#define THREAD_MONITOR_H

#include <pthread.h>
#include <sys/types.h>

typedef struct
{
    pthread_t thread_id;
    pid_t process_id;
    const char *thread_name;
    int active;
} ThreadInfo;

int thread_monitor_start(ThreadInfo *info);
void thread_monitor_stop(ThreadInfo *info);
void thread_monitor_print(const ThreadInfo *info);

#endif
