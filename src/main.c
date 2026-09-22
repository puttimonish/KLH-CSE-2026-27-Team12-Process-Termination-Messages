#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "process_manager.h"
#include "signal_handler.h"
#include "event_store.h"
#include "logger.h"
#include "memory_monitor.h"
#include "thread_monitor.h"
#include "dashboard.h"

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
    printf("\nStarting Phase 1 demonstration...\n");

    printf("\n[PROCESS MANAGER]\n");
    printf("Creating child process using fork()...\n");

    pid_t child = create_child_process();

    if (child < 0)
    {
        fprintf(stderr, "Failed to create child process.\n");
        return;
    }

    if (child == 0)
    {
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

    event_store_add(child,
                    getpid(),
                    "NORMAL_TERMINATION",
                    "Child exited normally");

    log_process_event(child,
                      getpid(),
                      "NORMAL_TERMINATION",
                      "Child exited normally");

    print_process_result(&result);
}

static void signal_termination_demo(void)
{
    printf("\nStarting Phase 2: Signal-Based Termination...\n");

    printf("\n============================================================\n");
    printf("             PHASE 2: SIGNAL TERMINATION\n");
    printf("============================================================\n");

    printf("\n[PROCESS MANAGER]\n");
    printf("Creating child process using fork()...\n");

    pid_t child = create_child_process();

    if (child < 0)
    {
        fprintf(stderr, "Failed to create child process.\n");
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

    printf("\n============================================================\n");
    printf("                 SIGNAL TERMINATION EVENT\n");
    printf("============================================================\n");
    printf("Process PID      : %d\n", child);
    printf("Signal           : SIGTERM (15)\n");
    printf("Termination      : Requested by parent\n");
    printf("============================================================\n");

    event_store_add(child,
                    getpid(),
                    "SIGNAL_TERMINATION",
                    "SIGTERM sent by parent");

    log_process_event(child,
                      getpid(),
                      "SIGNAL_TERMINATION",
                      "SIGTERM sent by parent");

    ProcessResult result = wait_for_process(child);

    printf("\nParent detected child termination.\n");

    print_process_result(&result);
}

int main(void)
{
    display_banner();

    event_store_init();
    logger_init();

    normal_process_demo();

    signal_termination_demo();

    event_store_print();

    printf("\n============================================================\n");
    printf("              PROJECT STATUS SUMMARY\n");
    printf("============================================================\n");

    printf("Phase 1 : Normal process termination       [COMPLETED]\n");
    printf("Phase 2 : Signal-based termination         [COMPLETED]\n");
    printf("Event Store :                            [ACTIVE]\n");
    printf("Process Logger :                         [ACTIVE]\n");
    printf("Recorded Events : %d\n", event_store_count());

    printf("============================================================\n");

    logger_close();

    printf("\nAll demonstrations completed successfully.\n");

    return 0;
}
