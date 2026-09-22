#ifndef EVENT_STORE_H
#define EVENT_STORE_H

#include <sys/types.h>

typedef struct
{
    int event_id;
    pid_t pid;
    pid_t parent_pid;
    char type[32];
    char details[128];
} ProcessEvent;

void event_store_init(void);

int event_store_add(pid_t pid,
                    pid_t parent_pid,
                    const char *type,
                    const char *details);

int event_store_count(void);

void event_store_print(void);

#endif
