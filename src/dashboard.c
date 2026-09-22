#include "dashboard.h"

#include <stdio.h>

void dashboard_print_header(void)
{
    printf("============================================================\n");
    printf("          PROCESS TERMINATION MONITOR\n");
    printf("       Linux Process Lifecycle System\n");
    printf("============================================================\n");
    printf("          Operating Systems & Systems Programming\n");
    printf("                    25CS2104E\n");
    printf("============================================================\n");
}

void dashboard_print_phase(const char *phase_name, const char *description)
{
    printf("\n");
    printf("------------------------------------------------------------\n");
    printf("%s\n", phase_name ? phase_name : "PHASE");
    printf("------------------------------------------------------------\n");
    printf("%s\n", description ? description : "");
    printf("------------------------------------------------------------\n");
}

void dashboard_print_child_state(pid_t parent_pid,
                                 pid_t child_pid,
                                 const char *state)
{
    printf("\n[PROCESS DASHBOARD]\n");
    printf("Parent PID         : %d\n", parent_pid);
    printf("Child PID          : %d\n", child_pid);
    printf("Observed State     : %s\n", state ? state : "UNKNOWN");
}

void dashboard_print_summary(int normal_events,
                             int signal_events,
                             int total_events)
{
    printf("\n============================================================\n");
    printf("              PROJECT STATUS SUMMARY\n");
    printf("============================================================\n");
    printf("Phase 1 : Normal process termination       [COMPLETED]\n");
    printf("Phase 2 : SIGTERM termination              [COMPLETED]\n");
    printf("Phase 3 : SIGKILL termination              [COMPLETED]\n");
    printf("IPC      : Parent/Child communication      [ACTIVE]\n");
    printf("Memory Monitor :                            [ACTIVE]\n");
    printf("Thread Monitor :                            [ACTIVE]\n");
    printf("Event Store :                              [ACTIVE]\n");
    printf("Process Logger :                           [ACTIVE]\n");
    printf("Normal Events : %d\n", normal_events);
    printf("Signal Events : %d\n", signal_events);
    printf("Recorded Events : %d\n", total_events);
    printf("============================================================\n");
}
