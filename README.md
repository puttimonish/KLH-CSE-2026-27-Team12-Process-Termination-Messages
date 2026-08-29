\# Process Termination Messages



\## Operating Systems and Systems Programming (25CS2104E)



\*\*Academic Year:\*\* 2026-27, Term-I



\*\*Project Title:\*\* Process Termination Messages



\## Team Members



| Roll Number | Student Name |

|---|---|

| 2520030395 | Monish Putti |

| 2520030560 | Nikhil Reddy |

| 2520030599 | Hemasai |



\## Supervisor



\*\*Dr. K. Anusha\*\*



\## Abstract



The project focuses on implementing a Linux-based process termination message system that demonstrates how processes can be created, monitored, and terminated using Operating Systems concepts. The system provides appropriate messages when a process terminates, helping users understand the process lifecycle and termination status. The project uses Linux process management mechanisms and system programming concepts to handle process creation, execution, and termination. It demonstrates the interaction between parent and child processes and the use of termination-related system calls and signals. The proposed solution is implemented in C/C++ on a Linux environment such as Ubuntu. The system aims to provide a simple and clear demonstration of process termination and communication between processes. The expected outcome is a functional Linux program that displays meaningful termination messages and provides practical understanding of process management, process states, signals, and system calls. This project helps connect theoretical Operating Systems concepts with their practical implementation in a Linux environment.



\## Problem Statement



In a Linux operating system, processes may terminate normally or due to signals and other termination conditions. Understanding how a parent process detects and responds to the termination of a child process is an important Operating Systems concept. The project aims to develop a Linux-based system that demonstrates process termination and displays appropriate termination messages. The system will use process management and system programming mechanisms to create processes, monitor their execution, detect their termination status, and communicate the result through meaningful messages.



\## Objectives



1\. To create and manage processes in a Linux environment.

2\. To demonstrate the termination of child processes and detection of their termination status.

3\. To use Linux system calls, signals, and process management mechanisms for handling process termination.

4\. To display meaningful termination messages and demonstrate practical Operating Systems concepts.



\## Proposed Methodology



The project will be designed and implemented in C/C++ on Linux/Ubuntu. The program will create and manage processes using Linux process management mechanisms. A parent process will create a child process and monitor its execution. The child process will perform a specified operation and terminate either normally or through a termination signal. The parent process will wait for the child process and obtain its termination status using appropriate Linux system calls. Based on the termination condition, the program will display a suitable process termination message. The implementation will demonstrate process creation, parent-child process interaction, process termination, signals, waiting, and termination-status handling.



\## Operating Systems Concepts / Linux APIs Used



| OS Concept / Linux API / System Call | Purpose |

|---|---|

| `fork()` | Creates a child process from the parent process. |

| `wait()` / `waitpid()` | Allows the parent process to wait for and obtain the termination status of the child process. |

| `exit()` | Terminates a process normally with an exit status. |

| `kill()` | Sends a signal to a process for demonstrating signal-based termination. |

| Signals | Used to handle and demonstrate process termination caused by signals. |



\## Tools / Platforms / Software Used



\- Linux / Ubuntu

\- C / C++

\- GCC Compiler

\- Bash / Linux Terminal

\- GitHub



\## Project Structure



```text

src/       Source code

docs/      Project documentation and technical notes

data/      Data resources or documented data-source reference

results/   Execution output, screenshots, and test results

reports/   Project reports and submission documents

README.md  Project overview and setup information

