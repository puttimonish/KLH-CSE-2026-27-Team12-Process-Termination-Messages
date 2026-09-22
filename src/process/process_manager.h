#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <sys/types.h>

typedef enum {
    TERMINATION_NORMAL,
    TERMINATION_SIGNAL,
    TERMINATION_UNKNOWN
} TerminationType;

typedef struct {
    pid_t pid;
    pid_t parent_pid;
    TerminationType type;
    int exit_status;
    int signal_number;
} ProcessResult;

pid_t create_child_process(void);
ProcessResult wait_for_process(pid_t child_pid);

void print_process_result(const ProcessResult *result);

#endif
