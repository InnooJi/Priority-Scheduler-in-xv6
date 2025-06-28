// User program that tests process priority system calls 

#include "kernel/types.h"
#include "user/user.h"
#include "../kernel/param.h"
#include "../kernel/syscall.h"

int mypid;
int ppid;

#define myassert(x) if (x) {} else { \
	  printf("%s: %d ", __FILE__, __LINE__); \
	  printf("assert failed (%s)\n", # x); \
	  kill(ppid); \
	  kill(mypid); \
	  exit(1); \
    }

int main() {
	mypid = getpid();
	ppid = -1;

#ifdef SYS_getprio
	int prio = getprio(mypid);
	printf("Parent priority is %d\n", prio);
	myassert(STARTPRIO == prio);
#ifdef SYS_setprio
	myassert(0 == setprio(STARTPRIO-1));
	prio = getprio(mypid);
	printf("Parent priority is now %d\n", prio);
	myassert(STARTPRIO-1 == prio);

	int child = fork();
	myassert(child >= 0);
	if (0 == child) {
		ppid = mypid;
		mypid = getpid();

		prio = getprio(mypid);
		printf("Child priority is %d\n", prio);
		myassert(STARTPRIO == prio);

		myassert(0 == setprio(MAXPRIO));
		prio = getprio(mypid);
		printf("Child priority is %d\n", prio);
		myassert(MAXPRIO == prio);

		prio = getprio(ppid);
		printf("Parent priority is %d\n", prio);
		myassert(STARTPRIO-1 == prio);

		exit(0);
	} else {
		while (child > 0) {
			int pid = wait(0);
			if (pid == child) {
				child = 0;
				prio = getprio(mypid);
				printf("Parent priority is %d\n", prio);
				myassert(STARTPRIO-1 == prio);
			}
		}
	}

	printf("PASSED\n");
    exit(0);
#else
	printf("Partially passed\n");
	printf("You must implement the setprio system call to run the full test\n");
    exit(1);
#endif
#else
	printf("You must implement the getprio system call\n");
    exit(1);
#endif
}