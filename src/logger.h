#ifndef LOGGER_H
#define LOGGER_H

#include <sys/types.h>

void logger_init(void);

void log_process_event(pid_t pid,
                       pid_t parent_pid,
                       const char *event_type,
                       const char *details);

void logger_close(void);

#endif
