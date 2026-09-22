#include "logging/event_store.h"

#include <stdio.h>
#include <string.h>

#define MAX_EVENTS 100

static ProcessEvent events[MAX_EVENTS];
static int event_count = 0;

void event_store_init(void)
{
    event_count = 0;
}

int event_store_add(pid_t pid,
                    pid_t parent_pid,
                    const char *type,
                    const char *details)
{
    if (event_count >= MAX_EVENTS)
        return -1;

    ProcessEvent *event = &events[event_count];

    event->event_id = event_count + 1;
    event->pid = pid;
    event->parent_pid = parent_pid;

    snprintf(event->type,
             sizeof(event->type),
             "%s",
             type ? type : "");

    snprintf(event->details,
             sizeof(event->details),
             "%s",
             details ? details : "");

    event_count++;

    return event->event_id;
}

int event_store_count(void)
{
    return event_count;
}

void event_store_print(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("                    EVENT STORE\n");
    printf("============================================================\n");

    if (event_count == 0)
    {
        printf("No events recorded.\n");
        printf("============================================================\n");
        return;
    }

    printf("%-5s %-8s %-8s %-18s %s\n",
           "ID",
           "PID",
           "PPID",
           "TYPE",
           "DETAILS");

    printf("------------------------------------------------------------\n");

    for (int i = 0; i < event_count; i++)
    {
        printf("%-5d %-8d %-8d %-18s %s\n",
               events[i].event_id,
               events[i].pid,
               events[i].parent_pid,
               events[i].type,
               events[i].details);
    }

    printf("============================================================\n");
}
