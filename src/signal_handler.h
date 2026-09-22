#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

void setup_signal_handlers(void);
void signal_handler(int signal_number);

#endif
