#include "process/process_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t create_child_process(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    return pid;
}

ProcessResult wait_for_process(pid_t child_pid)
{
    ProcessResult result;

    result.pid = child_pid;
    result.parent_pid = getpid();
    result.type = TERMINATION_UNKNOWN;
    result.exit_status = -1;
    result.signal_number = 0;

    int status;

    if (waitpid(child_pid, &status, 0) == -1) {
        perror("waitpid");
        return result;
    }

    if (WIFEXITED(status)) {
        result.type = TERMINATION_NORMAL;
        result.exit_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status)) {
        result.type = TERMINATION_SIGNAL;
        result.signal_number = WTERMSIG(status);
    }

    return result;
}

void print_process_result(const ProcessResult *result)
{
    printf("\n");
    printf("============================================================\n");
    printf("                 TERMINATION REPORT\n");
    printf("============================================================\n");

    printf("Process PID      : %d\n", result->pid);
    printf("Parent PID       : %d\n", result->parent_pid);

    if (result->type == TERMINATION_NORMAL) {
        printf("Termination      : NORMAL\n");
        printf("Exit Status      : %d\n", result->exit_status);
        printf("Signal           : NONE\n");
    }
    else if (result->type == TERMINATION_SIGNAL) {
        printf("Termination      : SIGNAL\n");
        printf("Exit Status      : N/A\n");
        printf("Signal Number    : %d\n", result->signal_number);
    }
    else {
        printf("Termination      : UNKNOWN\n");
    }

    printf("============================================================\n");
}
