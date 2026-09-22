#include "thread_monitor.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void *monitor_thread(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    info->process_id = getpid();
    info->thread_name = "Termination Monitor Thread";
    info->active = 1;

    while (info->active)
    {
        sleep(1);
    }

    return NULL;
}

int thread_monitor_start(ThreadInfo *info)
{
    if (!info)
        return -1;

    info->active = 0;
    info->process_id = getpid();
    info->thread_name = "Termination Monitor Thread";

    if (pthread_create(&info->thread_id,
                       NULL,
                       monitor_thread,
                       info) != 0)
    {
        return -1;
    }

    sleep(1);

    return 0;
}

void thread_monitor_stop(ThreadInfo *info)
{
    if (!info || !info->active)
        return;

    info->active = 0;
    pthread_join(info->thread_id, NULL);
}

void thread_monitor_print(const ThreadInfo *info)
{
    if (!info)
        return;

    printf("\n============================================================\n");
    printf("                    THREAD MONITOR\n");
    printf("============================================================\n");
    printf("Process PID        : %d\n", info->process_id);
    printf("Thread ID          : %lu\n",
           (unsigned long)info->thread_id);
    printf("Thread Name        : %s\n",
           info->thread_name ? info->thread_name : "Unknown");
    printf("Thread Status      : %s\n",
           info->active ? "ACTIVE" : "STOPPED");
    printf("============================================================\n");
}
