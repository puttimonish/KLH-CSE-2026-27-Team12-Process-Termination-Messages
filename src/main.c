
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "process/process_manager.h"
#include "logging/logger.h"
#include "logging/event_store.h"
#include "dashboard/dashboard.h"
#include "monitoring/process_inspector.h"
#include "monitoring/memory_monitor.h"
#include "monitoring/thread_monitor.h"
#include "process/signal_handler.h"
#include "communication/ipc.h"

#define COMMAND_SIZE 256
#define MAX_ARGUMENTS 32

static ThreadInfo monitor_info;

static pid_t child_pid = -1;
static int child_running = 0;
static int child_reaped = 0;
static int child_command_pipe[2] = {-1, -1};

static void record_event(const ProcessResult *result,
                         const char *type,
                         const char *details)
{
    event_store_add(result->pid, result->parent_pid, type, details);
    log_process_event(result->pid, result->parent_pid, type, details);
}

static void show_help(void)
{
    printf("\n========== PROCESS TERMINATION SHELL COMMANDS ==========\n");
    printf("help              - Show available commands\n");
    printf("system            - Show Linux system information\n");
    printf("refresh           - Request a SIGUSR1 monitoring refresh\n");
    printf("thread start      - Start the monitoring thread\n");
    printf("thread status     - Display monitoring thread status\n");
    printf("thread stop       - Stop and join the monitoring thread\n");
    printf("processes         - List live processes from /proc\n");
    printf("procinfo PID      - Inspect process memory and file descriptors\n");
    printf("memory PID        - Display process memory statistics\n");
    printf("memorytest        - Run the CO4 memory experiment\n");
    printf("iotest            - Run the CO5 file I/O experiment\n");
    printf("threadtest        - Run the CO6 synchronization experiment\n");
    printf("parent            - Display shell process information\n");
    printf("run COMMAND       - Execute a Linux command\n");
    printf("child             - Create a managed child process\n");
    printf("status            - Display child process status\n");
    printf("terminate normal  - Request normal child termination\n");
    printf("terminate sigterm - Terminate child using SIGTERM\n");
    printf("terminate sigkill - Force termination using SIGKILL\n");
    printf("wait              - Collect and display termination status\n");
    printf("events            - Display recorded termination events\n");
    printf("exit              - Exit the shell\n");
    printf("========================================================\n");
    printf("Examples: run ls, run pwd, run ps\n\n");
}

static void show_parent(void)
{
    printf("\n[PARENT PROCESS]\n");
    printf("PID        : %d\n", getpid());
    printf("Parent PID : %d\n", getppid());
    printf("Role       : Process Termination Shell\n");
}

static void run_linux_command(const char *input)
{
    char command_copy[COMMAND_SIZE];
    char *args[MAX_ARGUMENTS];
    int count = 0;

    if (strlen(input) >= sizeof(command_copy)) {
        printf("Command is too long.\n");
        return;
    }

    strcpy(command_copy, input);

    char *token = strtok(command_copy, " \t");

    while (token != NULL && count < MAX_ARGUMENTS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t");
    }

    args[count] = NULL;

    if (count == 0) {
        printf("Usage: run COMMAND [ARGUMENTS...]\n");
        return;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        execvp(args[0], args);
        perror("execvp");
        _exit(127);
    }

    int status;
    pid_t result;

    do {
        result = waitpid(pid, &status, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        perror("waitpid");
        return;
    }

    printf("\n[COMMAND EXECUTION REPORT]\n");
    printf("Command    : %s\n", args[0]);
    printf("Process ID : %d\n", pid);

    if (WIFEXITED(status)) {
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Signal     : %d\n", WTERMSIG(status));
    }
}

/* Run one of the standalone CO4-CO6 experiments. */
static void run_experiment(const char *name, const char *path)
{
    printf("\n[EXPERIMENT: %s]\n", name);
    run_linux_command(path);
}

static void create_child(void)
{
    if (child_running && !child_reaped) {
        printf("A child already exists. Check status or terminate it first.\n");
        return;
    }

    if (ipc_create(child_command_pipe) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        ipc_close(child_command_pipe[0]);
        ipc_close(child_command_pipe[1]);
        child_command_pipe[0] = -1;
        child_command_pipe[1] = -1;
        return;
    }

    if (pid == 0) {
        ipc_close(child_command_pipe[1]);

        char message[2] = {'\0', '\0'};
        int bytes_read = ipc_receive_message(
            child_command_pipe[0], message, sizeof(message));

        ipc_close(child_command_pipe[0]);

        if (bytes_read == 1 && message[0] == 'N') {
            _exit(0);
        }

        _exit(1);
    }

    ipc_close(child_command_pipe[0]);
    child_command_pipe[0] = -1;

    child_pid = pid;
    child_running = 1;
    child_reaped = 0;

    printf("\nChild process created successfully.\n");
    printf("Child PID : %d\n", child_pid);
    printf("Parent PID: %d\n", getpid());
    printf("State     : RUNNING\n");
}

static void show_status(void)
{
    if (child_pid == -1) {
        printf("No child exists. Use 'child' first.\n");
        return;
    }

    if (child_reaped) {
        printf("Child PID : %d\n", child_pid);
        printf("State     : TERMINATED (status already collected)\n");
        return;
    }

    int status;
    pid_t result = waitpid(child_pid, &status, WNOHANG);

    if (result == 0) {
        printf("\n[CHILD STATUS]\n");
        printf("PID   : %d\n", child_pid);
        printf("State : RUNNING\n");
    } else if (result == child_pid) {
        child_running = 0;
        child_reaped = 1;

        printf("\n[CHILD STATUS]\n");
        printf("PID   : %d\n", child_pid);

        if (WIFEXITED(status)) {
            printf("State       : TERMINATED NORMALLY\n");
            printf("Exit status : %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("State  : TERMINATED BY SIGNAL\n");
            printf("Signal : %d\n", WTERMSIG(status));
        }
    } else if (result == -1) {
        perror("waitpid");
    }
}

static void wait_for_child(void)
{
    if (child_pid == -1) {
        printf("No child exists. Use 'child' first.\n");
        return;
    }

    if (child_reaped) {
        printf("Termination status has already been collected.\n");
        return;
    }

    printf("Parent waiting for child PID %d...\n", child_pid);

    int status;
    pid_t result;

    do {
        result = waitpid(child_pid, &status, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        perror("waitpid");
        return;
    }

    child_running = 0;
    child_reaped = 1;

    ProcessResult process_result;
    process_result.pid = child_pid;
    process_result.parent_pid = getpid();
    process_result.exit_status = -1;
    process_result.signal_number = 0;

    if (WIFEXITED(status)) {
        process_result.type = TERMINATION_NORMAL;
        process_result.exit_status = WEXITSTATUS(status);

        record_event(&process_result,
                     "NORMAL_TERMINATION",
                     "Child exited normally");

        printf("\n[TERMINATION REPORT]\n");
        printf("Child PID   : %d\n", child_pid);
        printf("Termination : NORMAL\n");
        printf("Exit status : %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        char details[80];

        process_result.type = TERMINATION_SIGNAL;
        process_result.signal_number = WTERMSIG(status);

        snprintf(details, sizeof(details),
                 "Child terminated by signal %d", WTERMSIG(status));

        record_event(&process_result, "SIGNAL_TERMINATION", details);

        printf("\n[TERMINATION REPORT]\n");
        printf("Child PID   : %d\n", child_pid);
        printf("Termination : SIGNAL\n");
        printf("Signal      : %d\n", WTERMSIG(status));
    } else {
        process_result.type = TERMINATION_UNKNOWN;

        record_event(&process_result,
                     "UNKNOWN_TERMINATION",
                     "Unknown termination state");

        printf("Unknown child termination state.\n");
    }
}

static void terminate_child(const char *method)
{
    if (child_pid == -1 || !child_running || child_reaped) {
        printf("No running child. Use 'child' first.\n");
        return;
    }

    if (strcmp(method, "normal") == 0) {
        if (child_command_pipe[1] == -1) {
            printf("Normal termination channel is unavailable.\n");
            return;
        }

        const char message[] = {'N', '\0'};

        if (ipc_send_message(child_command_pipe[1], message) == -1) {
            perror("Sending normal termination request");
            return;
        }

        ipc_close(child_command_pipe[1]);
        child_command_pipe[1] = -1;

        printf("Normal termination request sent to PID %d.\n", child_pid);
        printf("Enter 'wait' to collect its status.\n");
    } else if (strcmp(method, "sigterm") == 0) {
        if (kill(child_pid, SIGTERM) == -1) {
            perror("kill(SIGTERM)");
            return;
        }

        if (child_command_pipe[1] != -1) {
            ipc_close(child_command_pipe[1]);
            child_command_pipe[1] = -1;
        }

        printf("SIGTERM sent to child PID %d.\n", child_pid);
        printf("Enter 'wait' to collect its status.\n");
    } else if (strcmp(method, "sigkill") == 0) {
        if (kill(child_pid, SIGKILL) == -1) {
            perror("kill(SIGKILL)");
            return;
        }

        if (child_command_pipe[1] != -1) {
            ipc_close(child_command_pipe[1]);
            child_command_pipe[1] = -1;
        }

        printf("SIGKILL sent to child PID %d.\n", child_pid);
        printf("Enter 'wait' to collect its status.\n");
    } else {
        printf("Unknown method. Use normal, sigterm, or sigkill.\n");
    }
}

static void show_events(void)
{
    printf("\n[RECORDED TERMINATION EVENTS]\n");
    event_store_print();
}

static void cleanup(void)
{
    if (child_running && !child_reaped && child_pid > 0) {
        if (child_command_pipe[1] != -1) {
            const char message[] = {'N', '\0'};

            if (ipc_send_message(child_command_pipe[1], message) == -1)
                perror("Sending shutdown termination request");

            ipc_close(child_command_pipe[1]);
            child_command_pipe[1] = -1;
        } else {
            (void)kill(child_pid, SIGTERM);
        }

        int status;

        while (waitpid(child_pid, &status, 0) == -1 &&
               errno == EINTR) {
        }
    }

    if (child_command_pipe[0] != -1) {
        ipc_close(child_command_pipe[0]);
        child_command_pipe[0] = -1;
    }

    if (child_command_pipe[1] != -1) {
        ipc_close(child_command_pipe[1]);
        child_command_pipe[1] = -1;
    }

    if (monitor_info.initialized) {
        thread_monitor_stop(&monitor_info);
    }
}

int main(void)
{
    char command[COMMAND_SIZE];

    dashboard_print_header();
    logger_init();
    event_store_init();
    setup_signal_handlers();

    printf("\n========================================\n");
    printf("       PROCESS TERMINATION SHELL\n");
    printf("========================================\n");
    printf("Type 'help' to see available commands.\n");

    while (1) {
        printf("\npts> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\n");
            break;
        }

        if (strchr(command, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Command too long.\n");
            continue;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "help") == 0) {
            show_help();
        } else if (strcmp(command, "thread start") == 0) {
            if (monitor_info.initialized) {
                printf("Thread monitor is already running or initialized.\n");
            } else if (thread_monitor_start(&monitor_info) == 0) {
                thread_monitor_print(&monitor_info);
                printf("Thread monitor started successfully.\n");
            } else {
                printf("Could not start thread monitor.\n");
            }
        } else if (strcmp(command, "thread status") == 0) {
            if (!monitor_info.initialized) {
                printf("Thread monitor is not running. Use 'thread start'.\n");
            } else {
                thread_monitor_print(&monitor_info);
            }
        } else if (strcmp(command, "thread stop") == 0) {
            if (!monitor_info.initialized) {
                printf("Thread monitor is not running.\n");
            } else {
                thread_monitor_stop(&monitor_info);
                printf("Thread monitor stopped and joined successfully.\n");
            }
        } else if (strcmp(command, "memorytest") == 0) {
            run_experiment("CO4 MEMORY MANAGEMENT", "./memory_experiment");
        } else if (strcmp(command, "iotest") == 0) {
            run_experiment("CO5 FILE SYSTEM AND I/O", "./file_io_experiment");
        } else if (strcmp(command, "threadtest") == 0) {
            run_experiment("CO6 THREAD SYNCHRONIZATION", "./thread_experiment");
        } else if (strcmp(command, "parent") == 0) {
            show_parent();
        } else if (strcmp(command, "refresh") == 0) {
            clear_signal_refresh_request();
            if (kill(getpid(), SIGUSR1) == -1) {
                perror("kill SIGUSR1");
            } else if (signal_refresh_requested()) {
                clear_signal_refresh_request();
                printf("\n[SIGUSR1 MONITORING REFRESH]\n");
                inspector_show_system();
                inspector_list_processes();
            } else {
                printf("SIGUSR1 was sent, but no refresh was recorded.\n");
            }
        } else if (strcmp(command, "system") == 0) {
            inspector_show_system();
        } else if (strcmp(command, "processes") == 0) {
            inspector_list_processes();
        } else if (strncmp(command, "memory ", 7) == 0) {
            char *end = NULL;
            errno = 0;
            long pid_value = strtol(command + 7, &end, 10);

            while (end != NULL && (*end == ' ' || *end == '\t'))
                end++;

            if (errno != 0 || end == command + 7 || end == NULL ||
                *end != '\0' || pid_value <= 0 ||
                pid_value > 2147483647L) {
                printf("Usage: memory PID\n");
            } else {
                MemoryInfo info;
                pid_t target_pid = (pid_t)pid_value;

                if (memory_monitor_read(target_pid, &info) == 0) {
                    memory_monitor_print(target_pid, &info);
                } else {
                    printf("Could not read memory information for PID %ld.\n",
                           pid_value);
                }
            }
        } else if (strncmp(command, "procinfo ", 9) == 0) {
            char *end = NULL;
            errno = 0;
            long pid_value = strtol(command + 9, &end, 10);

            while (end != NULL && (*end == ' ' || *end == '\t'))
                end++;

            if (errno != 0 || end == command + 9 || end == NULL ||
                *end != '\0' || pid_value <= 0 ||
                pid_value > 2147483647L) {
                printf("Usage: procinfo PID\n");
            } else {
                inspector_show_process((int)pid_value);
            }
        } else if (strncmp(command, "run ", 4) == 0) {
            run_linux_command(command + 4);
        } else if (strcmp(command, "child") == 0) {
            create_child();
        } else if (strcmp(command, "status") == 0) {
            show_status();
        } else if (strcmp(command, "wait") == 0) {
            wait_for_child();
        } else if (strcmp(command, "terminate normal") == 0) {
            terminate_child("normal");
        } else if (strcmp(command, "terminate sigterm") == 0) {
            terminate_child("sigterm");
        } else if (strcmp(command, "terminate sigkill") == 0) {
            terminate_child("sigkill");
        } else if (strcmp(command, "events") == 0) {
            show_events();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else if (command[0] == '\0') {
            continue;
        } else {
            printf("Unknown command: %s\n", command);
            printf("Type 'help' to see available commands.\n");
        }
    }

    cleanup();
    logger_close();
    printf("Exiting Process Termination Shell.\n");
    return 0;
}
