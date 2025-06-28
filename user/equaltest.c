// User program that tests priority scheduler with processes of equal priority

#include "kernel/types.h"
#include "user/user.h"
#include "../kernel/param.h"
#include "../kernel/syscall.h"

int start_time = -1;

#pragma GCC push_options
#pragma GCC optimize ("O0")
int spin() {
	int k = 0;
	for (int j = 1000*1000*6; j > 0; j--) {
		k += j % 2;
	}
	return k;
}
#pragma GCC pop_options

int runproc(char *id, int prio, int order) {
	int ppid = getpid();
	int pid = fork();
	if (pid < 0) {
		printf("FAIL: did not fork child%s\n", id);
		return pid;
	} else if (0 == pid) {
#ifdef SYS_setprio
		setprio(prio);
#endif
		sleep(1);

		pid = getpid();
		for (int i = 0; i < 9; i++) {
			int curr = uptime() - start_time;
			printf("Running %s (elapsed=%d)\n", id, curr);
			if (curr % 3 != order) {
				printf("FAIL: %s should not be running at time %d\n", id, curr);
				exit(1);
			}
			spin();
		}

		printf("Finished %s\n", id);
		spin();
		exit(0);
	} else {
		printf("child%s (pid=%d) forked by %d\n", id, pid, ppid);
	}

	return pid;
}


int main() {
#ifdef SYS_setprio
	setprio(MAXPRIO);
	start_time = uptime() + 1;

	int childA = runproc("A", MAXPRIO-1, 0);
	int childB = runproc("B", MAXPRIO-1, 1);
	int childC = runproc("C", MAXPRIO-1, 2);
	if (childA < 0 || childB < 0 || childC < 0) {
		kill(childA);
		kill(childB);
		kill(childC);
	}

	while (childA > 0 || childB > 0 || childC > 0) {
		int pid = wait(0);
		if (pid == childA) {
			childA = 0;
		} else if (pid == childB) {
			childB = 0;
		} else if (pid == childC) {
			childC = 0;
		}
	}

	printf("Test completed\n");
	printf("Process order should be: A, B, C, A, B, C, ...\n");
	printf("Check above output for failures\n");
    exit(0);
#else
	printf("You must implement the setprio system call\n");
    exit(1);
#endif
}