// User program that tests changes to xv6
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int main() {
	int pid = getpid();
    int prio;

    // Test 1: getprio for the current process
    prio = getprio(pid);
    printf("Test 1: getprio for the current process: %d\n", prio);

    // Test 2: setprio for the current process
    int new_prio = 3;
    if (setprio(new_prio) == 0) {
        prio = getprio(pid);
        printf("Test 2: setprio for the current process: %d\n", prio);
    } else {
        printf("Test 2: setprio for the current process failed\n");
    }

    // Test 3: getprio for another valid process
    int fork_pid = fork();
    if (fork_pid == 0) {
        // Child process
        int child_prio = getprio(getpid());
        printf("Test 3: getprio for child process: %d\n", child_prio);
        exit(0);
    } else {
        // Parent process
        wait(0);
        int child_prio = getprio(fork_pid);
        printf("Test 3: getprio for another valid process (child): %d\n", child_prio);
    }

    // Test 4: getprio for an invalid process
    prio = getprio(-1);
    printf("Test 4: getprio for an invalid process: %d\n", prio);

    // Test 5: setprio with an invalid priority
    int invalid_prio = MAXPRIO + 1;
    if (setprio(invalid_prio) == -1) {
        printf("Test 5: setprio with an invalid priority returned -1 as expected\n");
    } else {
        printf("Test 5: setprio with an invalid priority failed\n");
    }

    exit(0);
}