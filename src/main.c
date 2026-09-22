#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "process/process_manager.h"
#include "process/signal_handler.h"
#include "communication/ipc.h"
#include "monitoring/memory_monitor.h"
#include "monitoring/thread_monitor.h"
#include "logging/logger.h"
#include "logging/event_store.h"
#include "dashboard/dashboard.h"

static void record_event(const ProcessResult *result,
                         const char *type,
                         const char *details)
{
    event_store_add(
        result->pid,
        result->parent_pid,
        type,
        details
    );

    log_process_event(
        result->pid,
        result->parent_pid,
        type,
        details
    );
}

static void normal_process_demo(void)
{
    int ipc_fd[2];

    dashboard_print_phase(
        "PHASE 1: NORMAL TERMINATION",
        "fork() + IPC + memory monitoring + thread monitoring + waitpid()"
    );

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

        ThreadInfo thread_info;

        if (thread_monitor_start(&thread_info) == 0)
        {
            thread_monitor_print(&thread_info);
        }
        else
        {
            printf("Thread Monitor : FAILED TO START\n");
        }

        MemoryInfo memory;

        if (memory_monitor_read(getpid(), &memory) == 0)
        {
            memory_monitor_print(getpid(), &memory);
        }
        else
        {
            printf("Memory Monitor : UNAVAILABLE\n");
        }

        printf("\nChild is performing its task...\n");
        sleep(1);

        ipc_send_message(
            ipc_fd[1],
            "Child completed task and is terminating normally"
        );

        printf("Child is terminating normally...\n");

        thread_monitor_stop(&thread_info);

        ipc_close(ipc_fd[1]);
        exit(0);
    }

    ipc_close(ipc_fd[1]);

    dashboard_print_child_state(
        getpid(),
        child,
        "RUNNING - PARENT MONITORING"
    );

    char ipc_message[256];

    if (ipc_receive_message(ipc_fd[0], ipc_message, sizeof(ipc_message)) > 0)
    {
        printf("\n[IPC MESSAGE RECEIVED]\n");
        printf("From Child : %s\n", ipc_message);
    }

    ipc_close(ipc_fd[0]);

    ProcessResult result = wait_for_process(child);

    record_event(
        &result,
        "NORMAL_TERMINATION",
        "Child exited normally"
    );

    printf("\nParent detected child termination.\n");
    print_process_result(&result);
}

static void signal_termination_demo(void)
{
    dashboard_print_phase(
        "PHASE 2: SIGTERM TERMINATION",
        "Parent requests child termination using kill(SIGTERM)"
    );

    pid_t child = create_child_process();

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
        printf("Status     : Waiting for SIGTERM...\n");

        while (1)
        {
            printf("Child is alive and waiting...\n");
            sleep(1);
        }
    }

    dashboard_print_child_state(
        getpid(),
        child,
        "WAITING FOR SIGTERM"
    );

    sleep(2);

    printf("\nParent sending SIGTERM to child...\n");

    if (kill(child, SIGTERM) == -1)
    {
        perror("kill(SIGTERM) failed");
        return;
    }

    ProcessResult result = wait_for_process(child);

    print_process_result(&result);

    record_event(
        &result,
        "SIGNAL_TERMINATION",
        "SIGTERM sent by parent"
    );
}

static void sigkill_termination_demo(void)
{
    dashboard_print_phase(
        "PHASE 3: SIGKILL TERMINATION",
        "Parent forcefully terminates a child using kill(SIGKILL)"
    );

    pid_t child = create_child_process();

    if (child < 0)
    {
        perror("fork failed");
        return;
    }

    if (child == 0)
    {
        printf("\n[CHILD PROCESS]\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());
        printf("Status     : Running until forcefully terminated...\n");

        while (1)
        {
            printf("Child remains alive...\n");
            sleep(1);
        }
    }

    dashboard_print_child_state(
        getpid(),
        child,
        "RUNNING - TARGET FOR SIGKILL"
    );

    sleep(2);

    printf("\nParent sending SIGKILL to child...\n");

    if (kill(child, SIGKILL) == -1)
    {
        perror("kill(SIGKILL) failed");
        return;
    }

    ProcessResult result = wait_for_process(child);

    print_process_result(&result);

    record_event(
        &result,
        "SIGNAL_TERMINATION",
        "SIGKILL sent by parent"
    );
}

int main(void)
{
    dashboard_print_header();

    logger_init();
    event_store_init();

    normal_process_demo();
    signal_termination_demo();
    sigkill_termination_demo();

    event_store_print();

    dashboard_print_summary(
        1,
        2,
        event_store_count()
    );

    logger_close();

    printf("\nAll demonstrations completed successfully.\n");

    return 0;
}
