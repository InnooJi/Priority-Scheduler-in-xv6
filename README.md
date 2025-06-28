# Project 02: Priority Scheduler in xv6

## Overview
You will implement a priority scheduler in the xv6 kernel and test your scheduler with various workloads.

### ❗Important tips❗
* **Work on the project in short (1-2 hour) sessions over multiple days**
* **Consult the [xv6 book](https://pdos.csail.mit.edu/6.1810/2024/xv6/book-riscv-rev4.pdf)**
* **Leverage what you learned in labs 3 and 4**
* **Follow good [program design](#design), [testing, and debugging](#testing-and-debugging) practices**

## Getting started 
Clone your repository in your home directory on your assigned tiger server. Your repository includes a copy of xv6. Refer to lab 3 if you've forgotten how to compile and run xv6.

The scheduling policy currently implemented in xv6 is a poor approximation of round robin. The scheduler loops through the array of process control blocks (PCBs) until it finds a runnable process. The runnable process runs for one time quantum, or until it makes a system call that causes the process to become blocked (e.g., `wait`, `sleep`, `read`). The scheduler then continues to loop through the list of PCBs until it finds the next runnable process. Thus, processes are scheduled based on the order in which the OS stores processes' metadata, rather than the order in which the processes became ready.

Before you proceed you should try to understand the source code for the current scheduler: look at the `scheduler` function in `kernel/proc.c` and [Section 7.3](https://pdos.csail.mit.edu/6.1810/2024/xv6/book-riscv-rev4.pdf#page=73) in the xv6 book.

## Part 1: Setting a process's priority
Your first task is to assign a priority to each process, and add system calls that allow a process to set its own priority and get the priority of itself or another process.

### Initialize a process's priority
Two constants that impact a process's priority are defined in `kernel/param.h`:
* `MAXPRIO`—specifies the maximum priority level a process may have; the minimum priority is 0
* `STARTPRIO`—specifies the starting priority level of a process; the value of this constant should always be set such that MAXPRIO ≥ STARTPRIO ≥ 0

You must modify xv6 to:
* Store a process's priority in its process control block (PCB)
* Initialize a process's priority to `STARTPRIO` when the process is created

### Systm calls
To allow a process to determine its priority or the priority of another process, add a new system call:
```C
int getprio(int pid);
```
If the pid is invalid, the system call should return -1 to indicate a failure. Otherwise it should return the current priority of the specified process.

To allow a process to change its priority, add another new system call that sets the priority of the currently running process:
```C
int setprio(int priority);
```
If the priority is invalid, the system call should return -1. Otherwise, it should return 0 to indicate success.

To add these system calls, you’ll need to make modifications similar to those you made in Lab 3 to add the `childcount` system call.

### Testing and debugging
You can run the `priotest` program in the xv6 shell to test your system calls. You can also write your own tests in `user/mytests.c` and run this program in xv6.

You can use printf and gdb (follow the instructions from lab 4) to help debug your code.

## Part 2: Changing the scheduler

Now that processes have explicit priorities, you must modify xv6 to schedule processes according to their priorities—i.e., it should follow rules 1 and 2 presented in [Chapter 8 of OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-mlfq.pdf):
1. _If Priority(A) > Priority(B), A runs (B doesn’t)._
2. _If Priority(A) = Priority(B), A & R run in round robin (RR)._

The time quantum used for RR scheduling is one clock tick (see the `clockintr` function in `kernel/trap.c` to learn how the clock is incremented). 

### Implementation details
You should add a `prioscheduler` function to `kernel/proc.c` that implements your priority scheduler. The function should have the same prototype as the `scheduler` function in `kernel/proc.c`. You should add a prototype for your `prioscheduler` function to the `kernel/defs.h` header file. You may add additional helper functions if needed.

You'll need to add one or more global variables in `kernel/proc.c`. You may also need to add one or more fields to the `proc` structure defined in `kernel/proc.h`. Think about what information you need to track in order to know which process to run next.

Your scheduler should not attempt to run a process while it's sleeping (e.g., waiting for I/O). However, when a process becomes runnable—e.g., it's done sleeping or it's finished being created—you should ensure it has an opportunity to be chosen by your scheduler.

After you have (partially) implemented your priority scheduler, you should replace the call to `scheduler` in the `main` function in `kernel/main.c` with a call to your `prioscheduler` function.

### Testing and debugging
You can run the programs `equaltest` and `difftest` in the xv6 shell to test your scheduler. You can also write your own tests in `user/mytests.c` and run this program in xv6.

Note that the `equaltest` program may ocassionally fail due to a small amount of extra delay before the child process start running; this is not a problem as long as most invocations of `equaltest` operate correctly.

See [Part 1](#testing-and-debugging) for guidance on debugging.

## Functionality
A project whose functionality is **satisfactory** must have two or fewer minor bugs, such as:
* A process's starting priority is wrong
* The `getprio` system call doesnot return -1 if the PID is invalid
* The `setprio` system call does not return -1 if the priority is invalid
* A process that is killed is not properly terminated

A project whose functionality **needs minor improvements** must have two or fewer major bugs, such as:
* The `getprio` system call gets the wrong process's priority 
* The `setprio` system call sets the wrong process's priority
* A process is never scheduled to run
* The scheduler attempts to run a non-runnable (e.g., `UNUSED` or `SLEEPING`) process
* Processes of equal priority are not run in round robin (RR) order
* Processes of higher priority are not preferred
* A kernel panic occurs due to a locking issue

## Design

A project whose design is **satisfactory** must adhere to **all of the following**:
* **No errors or warnings during compilation:** You may be tempted to ignore compiler warnings, but they almost always mean there is a bug in your code.
* **Use appropriate data structures:** For example, implement a queue as a linked data structure
* **Ensure necessary locks are held:** For example, when accessing specific fields in `struct proc`, ensure the requisite lock specified in the comments in the `struct` definition in `kernel/proc.h` is `acquire`d and `release`d.
* **Avoid duplicated code:** Do not repeat the same (or very similar) code in multiple places in a function or in multiple functions. Instead, put the repeated code into a separate "helper" function that is called by other functions that rely on this code.
* **Include comments:** Each function you add must be preceded by a short comment that describes what the function does. More complex functions (e.g., `prioscheduler`) should also include comments within the function.

A project that adheres to **half to three-quarters of the above** has a design that **needs minor improvements**.

## Submission instructions
You should **commit and push** your updated files to your git repository. However, do not wait until your entire implementation is working before you commit it to your git repository; you should commit your code each time you write and debug a piece of functionality. 

## Acknowledgemnts
This project is inspired by the _xv6 Scheduler_ project developed by Remzi and Andrea Arpaci-Dusseau at the University of Wisconsin-Madison and the _Priority Scheduling and Preemption_ project developed by Dennis Brylow at Marquette University.