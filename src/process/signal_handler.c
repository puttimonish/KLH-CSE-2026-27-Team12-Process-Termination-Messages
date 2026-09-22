#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "process/signal_handler.h"

void signal_handler(int signal_number)
{
    printf("\n============================================================\n");
    printf("                 SIGNAL TERMINATION EVENT\n");
    printf("============================================================\n");

    printf("Process PID      : %d\n", getpid());

    if (signal_number == SIGTERM)
    {
        printf("Signal           : SIGTERM (%d)\n", signal_number);
        printf("Termination      : Requested by parent\n");
    }
    else if (signal_number == SIGINT)
    {
        printf("Signal           : SIGINT (%d)\n", signal_number);
        printf("Termination      : Interrupted by user\n");
    }
    else if (signal_number == SIGUSR1)
    {
        printf("Signal           : SIGUSR1 (%d)\n", signal_number);
        printf("Termination      : Custom monitoring event\n");
    }
    else
    {
        printf("Signal           : %d\n", signal_number);
    }

    printf("============================================================\n");

    /*
     * Restore the default action and re-raise the signal.
     * This allows the parent process to detect that the
     * child actually terminated because of a signal.
     */
    signal(signal_number, SIG_DFL);
    raise(signal_number);
}

void setup_signal_handlers(void)
{
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGUSR1, signal_handler);
}
