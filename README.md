# Process Termination Monitor

A Linux-based Operating Systems and Systems Programming project that demonstrates
process creation, process monitoring, communication, termination, and event
recording using C and POSIX system programming concepts.

---

## Course Information

- **Course:** Operating Systems & Systems Programming
- **Course Code:** 25CS2104E
- **Team:** KLH-CSE-2026-27-Team12

---

## Project Objective

The objective of this project is to demonstrate how a parent process creates,
monitors, communicates with, and detects the termination of child processes.

The system demonstrates three termination scenarios:

1. Normal process termination
2. SIGTERM-based termination
3. SIGKILL-based termination

In addition, the project implements:

- Parent-child Inter-Process Communication (IPC)
- Memory monitoring
- Thread monitoring
- Process status dashboard
- Event storage
- Persistent process-event logging

---

## Technologies Used

- **Language:** C
- **Operating System:** Linux
- **Compiler:** GCC
- **Build System:** Make
- **Process Management:** `fork()`, `waitpid()`
- **Signals:** `SIGTERM`, `SIGKILL`
- **IPC:** POSIX pipe
- **Threads:** POSIX Threads (`pthread`)
- **Memory Monitoring:** Linux `/proc` interface
- **Version Control:** Git and GitHub

---

## Project Structure

```text
KLH-CSE-2026-27-Team12-Process-Termination-Messages/
│
├── src/
│   ├── process/
│   │   ├── process_manager.c
│   │   ├── process_manager.h
│   │   ├── signal_handler.c
│   │   └── signal_handler.h
│   │
│   ├── communication/
│   │   ├── ipc.c
│   │   └── ipc.h
│   │
│   ├── monitoring/
│   │   ├── memory_monitor.c
│   │   ├── memory_monitor.h
│   │   ├── thread_monitor.c
│   │   └── thread_monitor.h
│   │
│   ├── logging/
│   │   ├── logger.c
│   │   ├── logger.h
│   │   ├── event_store.c
│   │   └── event_store.h
│   │
│   ├── dashboard/
│   │   ├── dashboard.c
│   │   └── dashboard.h
│   │
│   └── main.c
│
├── data/
│   └── process_events.log
│
├── docs/
│   └── OSSP_Abstract.pdf
│
├── reports/
├── results/
├── Makefile
├── README.md
└── .gitignore
