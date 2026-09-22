#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "signal_handler.h"
#include "process_manager.h"

static void display_banner(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("          PROCESS TERMINATION MONITOR\n");
    printf("       Linux Process Lifecycle System\n");
    printf("============================================================\n");
    printf("          Operating Systems & Systems Programming\n");
    printf("                    25CS2104E\n");
    printf("============================================================\n");
}

static void normal_process_demo(void)
{
    printf("\n[PROCESS MANAGER]\n");
    printf("Creating child process using fork()...\n");

    pid_t child = create_child_process();

    if (child < 0) {
        fprintf(stderr, "Failed to create child process.\n");
        return;
    }

    if (child == 0) {
        printf("\n[CHILD PROCESS]\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Status     : Running\n");

        printf("\nChild is performing its task...\n");
        sleep(2);

        printf("Child is terminating normally...\n");

        exit(0);
    }

    printf("\n[PARENT PROCESS]\n");
    printf("PID        : %d\n", getpid());
    printf("Child PID  : %d\n", child);
    printf("Status     : Monitoring child...\n");

    ProcessResult result = wait_for_process(child);

    printf("\nParent detected child termination.\n");

    print_process_result(&result);
}

static void signal_termination_demo(void)
{
    pid_t child;

    printf("\n============================================================\n");
    printf("             PHASE 2: SIGNAL TERMINATION\n");
    printf("============================================================\n");

    printf("\n[PROCESS MANAGER]\n");
    printf("Creating child process using fork()...\n");

    child = fork();

    if (child < 0)
    {
        perror("fork");
        return;
    }

    if (child == 0)
    {
        printf("\n[CHILD PROCESS]\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Status     : Waiting for termination signal...\n");

        setup_signal_handlers();

        while (1)
        {
            printf("Child is alive and waiting...\n");
            sleep(2);
        }
    }

    printf("\n[PARENT PROCESS]\n");
    printf("PID        : %d\n", getpid());
    printf("Child PID  : %d\n", child);
    printf("Status     : Monitoring child...\n");

    sleep(3);

    printf("\nParent sending SIGTERM to child...\n");

    if (kill(child, SIGTERM) == -1)
    {
        perror("kill");
        return;
    }

    ProcessResult result = wait_for_process(child);

    printf("\nParent detected child termination.\n");

    print_process_result(&result);
}
int main(void)
{
    display_banner();

    printf("\nStarting Phase 1 demonstration...\n");

normal_process_demo();

printf("\nStarting Phase 2: Signal-Based Termination...\n");

signal_termination_demo();

printf("\nAll demonstrations completed successfully.\n");
    return 0;
}
