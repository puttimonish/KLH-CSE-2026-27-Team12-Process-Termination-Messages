\# 🛑 Process Termination Messages



> ### 💻 Operating Systems and Systems Programming — 25CS2104E

> \*\*Academic Year:\*\* 2026–27 · \*\*Term:\*\* I



\---



\## 📌 Project Overview



\*\*Process Termination Messages\*\* is a Linux-based Operating Systems project that demonstrates how processes are \*\*created, monitored, and terminated\*\*.



The project focuses on the interaction between \*\*parent and child processes\*\*, process termination conditions, signals, and Linux system calls. The system displays meaningful messages based on how a child process terminates.



\---



\## 👥 Team Members



| 🆔 Roll Number | 👤 Student Name |

|:---:|:---|

| `2520030395` | \*\*Monish Putti\*\* |

| `2520030560` | \*\*Nikhil Reddy\*\* |

| `2520030599` | \*\*Hemasai\*\* |



\### 👨‍🏫 Supervisor



\*\*Dr. K. Anusha\*\*



\---



\## 🎯 Objectives



The main objectives of this project are:



\- 🐧 Create and manage processes in a Linux environment.

\- 🔄 Demonstrate the creation and termination of child processes.

\- 🔍 Detect and interpret child-process termination status.

\- ⚙️ Use Linux system calls and signals for process management.

\- 💬 Display meaningful messages based on termination conditions.

\- 📚 Connect theoretical Operating Systems concepts with practical implementation.



\---



\## 📝 Abstract



The project focuses on implementing a Linux-based process termination message system that demonstrates how processes can be created, monitored, and terminated using Operating Systems concepts.



The system provides appropriate messages when a process terminates, helping users understand the process lifecycle and termination status. The system uses Linux process management mechanisms and system programming concepts to handle process creation, execution, and termination.



It demonstrates the interaction between parent and child processes and the use of termination-related system calls and signals.



The proposed solution is implemented in \*\*C/C++\*\* on a Linux environment such as \*\*Ubuntu\*\*. The system aims to provide a simple and clear demonstration of process termination and communication between processes.



The expected outcome is a functional Linux program that displays meaningful termination messages and provides practical understanding of \*\*process management, process states, signals, and system calls\*\*.



\---



\## ❓ Problem Statement



In a Linux operating system, processes may terminate normally or due to signals and other termination conditions.



Understanding how a parent process detects and responds to the termination of a child process is an important Operating Systems concept.



\### 💡 Problem



The project aims to develop a Linux-based system that:



1\. Creates and manages processes.

2\. Monitors child-process execution.

3\. Detects the termination condition.

4\. Determines whether the process terminated normally or due to a signal.

5\. Displays an appropriate termination message.



\---



\## 🛠️ Proposed Methodology



The project will be designed and implemented in \*\*C/C++ on Linux/Ubuntu\*\*.



The basic workflow is:



```text

&#x20;               ┌───────────────────┐

&#x20;               │   Parent Process  │

&#x20;               └─────────┬─────────┘

&#x20;                         │

&#x20;                      fork()

&#x20;                         │

&#x20;               ┌─────────▼─────────┐

&#x20;               │   Child Process   │

&#x20;               └─────────┬─────────┘

&#x20;                         │

&#x20;                ┌────────▼────────┐

&#x20;                │    Execution    │

&#x20;                └────────┬────────┘

&#x20;                         │

&#x20;                ┌────────▼────────┐

&#x20;                │   Termination   │

&#x20;                └────────┬────────┘

&#x20;                         │

&#x20;                   exit / signal

&#x20;                         │

&#x20;               ┌─────────▼─────────┐

&#x20;               │   Parent waits    │

&#x20;               │   \& checks status │

&#x20;               └─────────┬─────────┘

&#x20;                         │

&#x20;               ┌─────────▼─────────┐

&#x20;               │ Termination       │

&#x20;               │ Message Displayed │

&#x20;               └───────────────────┘

```



\### 🔄 Process Flow



1\. 👨‍💻 The parent process starts.

2\. 🧬 The parent creates a child process using `fork()`.

3\. ⚙️ The child performs its specified operation.

4\. 🛑 The child terminates normally or through a signal.

5\. ⏳ The parent waits for the child using `wait()` or `waitpid()`.

6\. 🔍 The parent examines the termination status.

7\. 💬 A suitable termination message is displayed.



\---



\## 🧠 Operating Systems Concepts \& Linux APIs



| ⚙️ API / Concept | 📖 Purpose |

|---|---|

| `fork()` | 🧬 Creates a child process from the parent process. |

| `wait()` | ⏳ Allows the parent to wait for a child process. |

| `waitpid()` | 🔍 Waits for a specific child and obtains its termination status. |

| `exit()` | 🛑 Terminates a process normally with an exit status. |

| `kill()` | 📡 Sends a signal to a process. |

| Signals | 🚨 Demonstrate termination caused by signals. |



\---



\## 🧰 Tools \& Technologies



| 🔧 Tool | 📌 Purpose |

|---|---|

| 🐧 \*\*Ubuntu / Linux\*\* | Development and execution environment |

| 💻 \*\*C / C++\*\* | Programming language |

| ⚙️ \*\*GCC\*\* | Compilation |

| 🖥️ \*\*Bash / Linux Terminal\*\* | Program execution and testing |

| 🐙 \*\*Git \& GitHub\*\* | Version control and collaboration |



\---



\## 📂 Project Structure



```text

KLH-CSE-2026-27-Team12-Process-Termination-Messages/

│

├── 📁 src/

│   └── Source code

│

├── 📁 docs/

│   └── Project documentation and technical notes

│

├── 📁 data/

│   └── Data resources or documented data-source reference

│

├── 📁 results/

│   └── Execution output, screenshots, and test results

│

├── 📁 reports/

│   └── Project reports and submission documents

│

└── 📄 README.md

&#x20;   └── Project overview and setup information

```



\---



\## ▶️ Setup \& Execution



\### 1️⃣ Clone the Repository



```bash

git clone https://github.com/puttimonish/KLH-CSE-2026-27-Team12-Process-Termination-Messages.git

```



\### 2️⃣ Navigate to the Project



```bash

cd KLH-CSE-2026-27-Team12-Process-Termination-Messages

```



\### 3️⃣ Navigate to Source Code



```bash

cd src

```



\### 4️⃣ Compile the Program



```bash

gcc process\_termination.c -o process\_termination

```



\### 5️⃣ Run the Program



```bash

./process\_termination

```



> ⚠️ \*\*Note:\*\* The exact compilation and execution commands may be updated as the implementation develops.



\---



\## 🧪 Expected Demonstration



The project will demonstrate different process termination scenarios such as:



\### ✅ Normal Termination



```text

Child process started...

Child process completed successfully.



Parent process:

Child terminated normally.

Exit status: 0

```



\### 🚨 Signal-Based Termination



```text

Child process started...



Parent process:

Child terminated by signal.

Termination signal detected.

```



> The exact output will depend on the final implementation.



\---



\## 📊 Expected Outcomes



By completing this project, the team expects to demonstrate:



\- 🧬 Process creation using `fork()`

\- 👨‍👦 Parent-child process interaction

\- ⏳ Process synchronization using `wait()` / `waitpid()`

\- 🛑 Normal process termination

\- 🚨 Signal-based termination

\- 🔍 Termination-status detection

\- 💬 Meaningful process termination messages

\- 🐧 Practical Linux system programming



\---



\## 📈 Current Phase Status



> 🟢 \*\*Phase 1 — Project Setup \& Documentation\*\*



| Component | Status |

|---|:---:|

| 📁 Repository structure | ✅ Complete |

| 📄 README documentation | ✅ Complete |

| 🐧 Linux environment | 🔄 In progress |

| 💻 Source-code implementation | ⏳ Pending |

| 🧪 Testing | ⏳ Pending |

| 📊 Results | ⏳ Pending |

| 📑 Final report | ⏳ Pending |



\---



\## 🔐 Repository Guidelines



This repository follows the project requirements:



\- 👥 Each team member contributes using their own GitHub account.

\- 📈 Contributions are made progressively throughout the project.

\- 🏷️ Phase deliverables will be tagged appropriately.

\- 🔒 Credentials and API keys will not be committed.

\- 📂 Required project folders are maintained throughout development.

\- 👨‍🏫 Repository access will be provided to the supervisor and Course Coordinator.

\- 🔗 The repository will remain accessible until final evaluation.



\---



\## 🏷️ Project Information



| 📌 Field | 📄 Details |

|---|---|

| \*\*Course\*\* | Operating Systems and Systems Programming |

| \*\*Course Code\*\* | 25CS2104E |

| \*\*Academic Year\*\* | 2026–27 |

| \*\*Term\*\* | I |

| \*\*Team\*\* | Team 12 |

| \*\*Project\*\* | Process Termination Messages |

| \*\*Environment\*\* | Linux / Ubuntu |

| \*\*Language\*\* | C / C++ |

| \*\*Supervisor\*\* | Dr. K. Anusha |



\---



\## 🚀 Future Development



The project can be extended with:



\- 📋 Multiple termination scenarios

\- 📡 More signal-handling demonstrations

\- 🖥️ Improved terminal output

\- 📊 Detailed termination-status reporting

\- 🧪 Automated test cases

\- 📸 Execution screenshots and demonstrations

\- 📚 Additional technical documentation



\---



<div align="center">



\### 🛑 Process Termination Messages



\*\*Operating Systems and Systems Programming — 25CS2104E\*\*



💻 Built with C/C++ · 🐧 Linux · 🐙 GitHub



\*\*Team 12 · Academic Year 2026–27\*\*



</div>

