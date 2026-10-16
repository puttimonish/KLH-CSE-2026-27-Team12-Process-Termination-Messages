#define _DEFAULT_SOURCE

#include "process_inspector.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

static int numeric_name(const char *name)
{
    if (name == NULL || *name == '\0')
        return 0;

    for (const char *p = name; *p; ++p) {
        if (*p < '0' || *p > '9')
            return 0;
    }

    return 1;
}

static int read_process_stat(
    int pid,
    char *name,
    size_t name_size,
    char *state,
    int *ppid,
    unsigned long *rss_pages)
{
    char path[PATH_MAX];
    char buffer[8192];

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *file = fopen(path, "r");
    if (file == NULL)
        return -1;

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fclose(file);
        return -1;
    }
    fclose(file);

    /*
     * The process name is enclosed in parentheses and may contain
     * spaces. Find the final ')' before parsing the remaining fields.
     */
    char *left = strchr(buffer, '(');
    char *right = strrchr(buffer, ')');

    if (left == NULL || right == NULL || right <= left)
        return -1;

    size_t length = (size_t)(right - left - 1);
    if (length >= name_size)
        length = name_size - 1;

    memcpy(name, left + 1, length);
    name[length] = '\0';

    int parent = 0;
    char process_state = '?';
    unsigned long resident_pages = 0;

    /*
     * After the final ')': state is field 3, PPID field 4,
     * and RSS pages field 24. The first remaining token is state.
     */
    int parsed = sscanf(
        right + 1,
        " %c %d %*d %*d %*d %*u %*u %*u %*u %*u %*u "
        "%*u %*u %*d %*d %*d %*d %*d %*d %*u %*u %lu",
        &process_state,
        &parent,
        &resident_pages
    );

    if (parsed < 2)
        return -1;

    *state = process_state;
    *ppid = parent;

    if (rss_pages != NULL)
        *rss_pages = resident_pages;

    return 0;
}

void inspector_show_system(void)
{
    struct utsname system_info;

    printf("\n========== LINUX SYSTEM INFORMATION ==========\n");

    if (uname(&system_info) == 0) {
        printf("Operating system : %s\n", system_info.sysname);
        printf("Hostname         : %s\n", system_info.nodename);
        printf("Kernel release   : %s\n", system_info.release);
        printf("Machine          : %s\n", system_info.machine);
    } else {
        perror("uname");
    }

    long page_size = sysconf(_SC_PAGESIZE);
    long online_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    long total_pages = sysconf(_SC_PHYS_PAGES);
    long available_pages = sysconf(_SC_AVPHYS_PAGES);

    printf("Page size        : %ld bytes\n", page_size);
    printf("Online CPUs      : %ld\n", online_cpus);

    if (page_size > 0 && total_pages > 0) {
        printf("Physical RAM     : %.2f MiB\n",
               (double)total_pages * (double)page_size / 1048576.0);
    }

    if (page_size > 0 && available_pages >= 0) {
        printf("Available RAM    : %.2f MiB\n",
               (double)available_pages * (double)page_size / 1048576.0);
    }

    printf("==============================================\n");
}

void inspector_list_processes(void)
{
    DIR *directory = opendir("/proc");

    if (directory == NULL) {
        perror("Cannot open /proc");
        return;
    }

    printf("\n================ LIVE PROCESS LIST ================\n");
    printf("%-8s %-8s %-7s %s\n", "PID", "PPID", "STATE", "NAME");
    printf("---------------------------------------------------\n");

    struct dirent *entry;
    int displayed = 0;

    while ((entry = readdir(directory)) != NULL) {
        if (!numeric_name(entry->d_name))
            continue;

        char *end = NULL;
        long value = strtol(entry->d_name, &end, 10);

        if (end == entry->d_name || *end != '\0' ||
            value <= 0 || value > INT_MAX)
            continue;

        int pid = (int)value;
        char name[256];
        char state;
        int ppid;
        unsigned long rss_pages;

        if (read_process_stat(
                pid, name, sizeof(name), &state, &ppid, &rss_pages) == 0) {
            printf("%-8d %-8d %-7c %s\n", pid, ppid, state, name);
            displayed++;
        }
    }

    closedir(directory);

    printf("---------------------------------------------------\n");
    printf("Processes displayed: %d\n", displayed);
    printf("Note: processes may disappear while /proc is scanned.\n");
    printf("===================================================\n");
}

void inspector_show_process(int pid)
{
    if (pid <= 0) {
        printf("PID must be a positive integer.\n");
        return;
    }

    char name[256];
    char state;
    int ppid;
    unsigned long rss_pages;

    if (read_process_stat(
            pid, name, sizeof(name), &state, &ppid, &rss_pages) != 0) {
        printf("Cannot inspect PID %d: process may have exited or access is denied.\n",
               pid);
        return;
    }

    long page_size = sysconf(_SC_PAGESIZE);
    double rss_mib = 0.0;

    if (page_size > 0)
        rss_mib = (double)rss_pages * (double)page_size / 1048576.0;

    char fd_path[PATH_MAX];
    snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd", pid);

    DIR *fd_directory = opendir(fd_path);
    int fd_count = 0;

    if (fd_directory != NULL) {
        struct dirent *entry;

        while ((entry = readdir(fd_directory)) != NULL) {
            if (numeric_name(entry->d_name))
                fd_count++;
        }

        closedir(fd_directory);
    }

    printf("\n========== PROCESS DETAILS ==========\n");
    printf("PID                : %d\n", pid);
    printf("Parent PID         : %d\n", ppid);
    printf("Process name       : %s\n", name);
    printf("State              : %c\n", state);
    printf("Resident memory RSS : %.2f MiB\n", rss_mib);

    if (fd_directory == NULL) {
        printf("Open file descriptors: unavailable (permission or process access)\n");
    } else {
        printf("Open file descriptors: %d\n", fd_count);
    }

    printf("=====================================\n");
}
