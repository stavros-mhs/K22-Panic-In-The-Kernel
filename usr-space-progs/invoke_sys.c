#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

struct k22info {
	char comm[64];              /* name of the executable */
	pid_t pid;                  /* process ID */
	pid_t parent_pid;           /* parent process ID */
	pid_t first_child_pid;      /* PID of first child */
	pid_t next_sibling_pid;     /* PID of next sibling */
	unsigned long nvcsw;        /* number of voluntary context switches */
	unsigned long nivcsw;       /* number of involuntary context switches */
	unsigned long start_time;   /* monotonic start time in nanoseconds */
};


int main(void) {
	struct k22info buf[100]; // Init buf with 10 entries
	int num_entries = 100;   // Request up to 10 items

	int ret = syscall(467, buf, &num_entries);
	printf("ret: %d\n", ret);
	if (ret < 0)
	    perror("syscall error:");
	else
	    // Print the received data (example)
	    for (int i = 0; i < num_entries; ++i) {
	        printf("PID: %d, COMM: %s\n", buf[i].pid, buf[i].comm);
	    }
	return 0;
}
