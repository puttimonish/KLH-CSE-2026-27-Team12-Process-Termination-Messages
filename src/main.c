#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "process_manager.h"
#include "signal_handler.h"
#include "ipc.h"
#include "memory_monitor.h"
#include "thread_monitor.h"
#include "logger.h"
#include "event_store.h"

static void normal_process_demo(void)
{
    int ipc_fd[2];

    printf("\nStarting Phase 1 demonstration...\n");

    if (ipc_create(ipc_fd) != 0)
    {
        perror("IPC creation failed");
        return;
    }

    pid_t child = create_child_process();

    if (child < 0)
    {
        perror("fork failed");
        ipc_close(ipc_fd[0]);
        ipc_close(ipc_fd[1]);
        return;
    }

    if (child == 0)
    {
        ipc_close(ipc_fd[0]);

        printf("\n[CHILD PROCESS]\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Status     : Running\n");

        MemoryInfo memory;

        if (memory_monitor_read(getpid(), &memory) == 0)
        {
            memory_monitor_print(getpid(), &memory);
        }

        printf("\nChild is performing its task...\n");
        sleep(1);

        ipc_send_message(
            ipc_fd[1],
            "Child completed task and is terminating normally"
        );

        printf("Child is terminating normally...\n");

        ipc_close(ipc_fd[1]);
        exit(0);
    }

    ipc_close(ipc_fd[1]);

    printf("\n[PARENT PROCESS]\n");
    printf("PID        : %d\n", getpid());
    printf("Child PID  : %d\n", child);
    printf("Status     : Monitoring child...\n");

    char ipc_message[256];

    if (ipc_receive_message(ipc_fd[0], ipc_message, sizeof(ipc_message)) > 0)
    {
        printf("\n[IPC MESSAGE RECEIVED]\n");
        printf("From Child : %s\n", ipc_message);
    }

    ipc_close(ipc_fd[0]);

    ProcessResult result = wait_for_process(child);

    event_store_add(
        result.pid,
        result.parent_pid,
        "NORMAL_TERMINATION",
        "Child exited normally"
    );

    log_process_event(
        result.pid,
        result.parent_pid,
        "NORMAL_TERMINATION",
        "Child exited normally"
    );

    printf("\nParent detected child termination.\n");

    printf("\n============================================================\n");
    printf("                 TERMINATION REPORT\n");
    printf("============================================================\n");
    print_process_result(&result);
}

static void signal_termination_demo(void)
{
    printf("\nStarting Phase 2: Signal-Based Termination...\n");

    printf("\n============================================================\n");
    printf("             PHASE 2: SIGNAL TERMINATION\n");
    printf("============================================================\n");

    pid_t child = fork();

    if (child < 0)
    {
        perror("fork failed");
        return;
    }

    if (child == 0)
    {
        setup_signal_handlers();

        printf("\n[CHILD PROCESS]\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Status     : Waiting for termination signal...\n");

        while (1)
        {
            printf("Child is alive and waiting...\n");
            sleep(1);
        }
    }

    printf("\n[PARENT PROCESS]\n");
    printf("PID        : %d\n", getpid());
    printf("Child PID  : %d\n", child);
    printf("Status     : Monitoring child...\n");

    sleep(2);

    printf("\nParent sending SIGTERM to child...\n");

    kill(child, SIGTERM);

    ProcessResult result = wait_for_process(child);

    printf("\n============================================================\n");
    printf("                 TERMINATION REPORT\n");
    printf("============================================================\n");
    print_process_result(&result);

    event_store_add(
        result.pid,
        result.parent_pid,
        "SIGNAL_TERMINATION",
        "SIGTERM sent by parent"
    );

    log_process_event(
        result.pid,
        result.parent_pid,
        "SIGNAL_TERMINATION",
        "SIGTERM sent by parent"
    );
}

int main(void)
{
    printf("============================================================\n");
    printf("          PROCESS TERMINATION MONITOR\n");
    printf("       Linux Process Lifecycle System\n");
    printf("============================================================\n");
    printf("          Operating Systems & Systems Programming\n");
    printf("                    25CS2104E\n");
    printf("============================================================\n");

    logger_init();
    event_store_init();

    normal_process_demo();

    signal_termination_demo();

    printf("\n");
    event_store_print();

    printf("\n============================================================\n");
    printf("              PROJECT STATUS SUMMARY\n");
    printf("============================================================\n");
    printf("Phase 1 : Normal process termination       [COMPLETED]\n");
    printf("Phase 2 : Signal-based termination         [COMPLETED]\n");
    printf("IPC      : Parent/Child communication      [ACTIVE]\n");
    printf("Memory Monitor :                            [ACTIVE]\n");
    printf("Event Store :                              [ACTIVE]\n");
    printf("Process Logger :                           [ACTIVE]\n");
    printf("Recorded Events : %d\n", event_store_count());
    printf("============================================================\n");

    logger_close();

    printf("\nAll demonstrations completed successfully.\n");

    return 0;
}
