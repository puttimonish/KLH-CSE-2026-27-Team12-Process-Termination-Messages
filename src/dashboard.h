#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <sys/types.h>

void dashboard_print_header(void);
void dashboard_print_phase(const char *phase_name, const char *description);
void dashboard_print_child_state(pid_t parent_pid,
                                 pid_t child_pid,
                                 const char *state);
void dashboard_print_summary(int normal_events,
                             int signal_events,
                             int total_events);

#endif
