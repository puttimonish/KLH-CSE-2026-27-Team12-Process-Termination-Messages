#include "monitoring/thread_monitor.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

static void *monitor_thread(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    pthread_mutex_lock(&info->mutex);
    info->process_id = getpid();
    info->thread_name = "Termination Monitor Thread";
    info->active = 1;
    pthread_cond_broadcast(&info->condition);

    while (info->active) {
        /*
         * Timed waiting demonstrates condition-variable synchronization
         * and allows the monitor to wake periodically.
         */
        struct timespec deadline;
        clock_gettime(CLOCK_REALTIME, &deadline);
        deadline.tv_sec++;

        int result = pthread_cond_timedwait(
            &info->condition, &info->mutex, &deadline);

        if (result != 0 && result != ETIMEDOUT) {
            fprintf(stderr, "Monitor condition wait failed: %s\n",
                    strerror(result));
            break;
        }
    }

    info->active = 0;
    pthread_mutex_unlock(&info->mutex);
    return NULL;
}

int thread_monitor_start(ThreadInfo *info)
{
    if (info == NULL)
        return -1;

    memset(info, 0, sizeof(*info));

    int result = pthread_mutex_init(&info->mutex, NULL);
    if (result != 0) {
        fprintf(stderr, "Mutex initialization failed: %s\n",
                strerror(result));
        return -1;
    }

    result = pthread_cond_init(&info->condition, NULL);
    if (result != 0) {
        fprintf(stderr, "Condition initialization failed: %s\n",
                strerror(result));
        pthread_mutex_destroy(&info->mutex);
        return -1;
    }

    info->initialized = 1;
    info->process_id = getpid();
    info->thread_name = "Termination Monitor Thread";

    result = pthread_create(&info->thread_id, NULL, monitor_thread, info);
    if (result != 0) {
        fprintf(stderr, "Thread creation failed: %s\n", strerror(result));
        pthread_cond_destroy(&info->condition);
        pthread_mutex_destroy(&info->mutex);
        info->initialized = 0;
        return -1;
    }

    pthread_mutex_lock(&info->mutex);
    while (!info->active)
        pthread_cond_wait(&info->condition, &info->mutex);
    pthread_mutex_unlock(&info->mutex);

    return 0;
}

void thread_monitor_stop(ThreadInfo *info)
{
    if (info == NULL || !info->initialized)
        return;

    pthread_mutex_lock(&info->mutex);
    info->active = 0;
    pthread_cond_broadcast(&info->condition);
    pthread_mutex_unlock(&info->mutex);

    pthread_join(info->thread_id, NULL);

    pthread_cond_destroy(&info->condition);
    pthread_mutex_destroy(&info->mutex);
    info->initialized = 0;
}

void thread_monitor_print(const ThreadInfo *info)
{
    if (info == NULL || !info->initialized)
        return;

    ThreadInfo *mutable_info = (ThreadInfo *)info;

    pthread_mutex_lock(&mutable_info->mutex);

    printf("\n========== THREAD MONITOR ==========\n");
    printf("Process PID : %d\n", mutable_info->process_id);
    printf("Thread ID   : %lu\n",
           (unsigned long)mutable_info->thread_id);
    printf("Thread Name : %s\n",
           mutable_info->thread_name != NULL
               ? mutable_info->thread_name : "Unknown");
    printf("Thread State: %s\n",
           mutable_info->active ? "ACTIVE" : "STOPPED");
    printf("Synchronization: mutex + condition variable\n");
    printf("====================================\n");

    pthread_mutex_unlock(&mutable_info->mutex);
}
