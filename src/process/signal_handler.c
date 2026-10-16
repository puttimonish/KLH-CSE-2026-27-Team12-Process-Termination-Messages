#include "process/signal_handler.h"
#include <stddef.h>

#include <signal.h>

static volatile sig_atomic_t refresh_requested = 0;

void signal_handler(int signal_number)
{
    if (signal_number == SIGUSR1) {
        refresh_requested = 1;
    }
}

int signal_refresh_requested(void)
{
    return refresh_requested != 0;
}

void clear_signal_refresh_request(void)
{
    refresh_requested = 0;
}

void setup_signal_handlers(void)
{
    struct sigaction action;

    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGUSR1, &action, NULL);

    /*
     * Preserve normal termination behavior for SIGTERM and SIGINT.
     * Their default actions terminate the receiving process.
     */
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
}
