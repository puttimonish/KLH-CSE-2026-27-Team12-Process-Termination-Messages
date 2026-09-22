#include "logger.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

static FILE *log_file = NULL;

void logger_init(void)
{
    mkdir("data", 0755);

    log_file = fopen("data/process_events.log", "a");

    if (!log_file)
    {
        perror("Unable to open log file");
        return;
    }

    fprintf(log_file,
            "\n================ PROCESS MONITOR SESSION ================\n");

    fflush(log_file);
}

void log_process_event(pid_t pid,
                       pid_t parent_pid,
                       const char *event_type,
                       const char *details)
{
    if (!log_file)
        return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char timestamp[64];

    strftime(timestamp,
             sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S",
             tm_info);

    fprintf(log_file,
            "[%s] PID=%d PPID=%d EVENT=%s DETAILS=%s\n",
            timestamp,
            pid,
            parent_pid,
            event_type,
            details);

    fflush(log_file);
}

void logger_close(void)
{
    if (log_file)
    {
        fprintf(log_file,
                "================ SESSION END =================\n\n");

        fclose(log_file);
        log_file = NULL;
    }
}
