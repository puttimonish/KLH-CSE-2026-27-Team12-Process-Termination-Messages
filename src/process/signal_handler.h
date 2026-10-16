#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

void setup_signal_handlers(void);
void signal_handler(int signal_number);

/* Set when SIGUSR1 requests a monitoring refresh. */
int signal_refresh_requested(void);
void clear_signal_refresh_request(void);

#endif
