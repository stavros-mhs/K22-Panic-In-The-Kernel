#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>

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

/* Find the position of a process with given parent_pid in the stack.
 * Returns the index in the stack, or -1 if not found.
 * The stack contains positions (index) of processes in the buffer.
 */
static int find_parent(int *stack, int top, int parent_pid, struct k22info *buf)
{
        for (int i = top; i >= 0; i--) {
                if (buf[stack[i]].pid == parent_pid) {
                        return i;
                }
        }
        return -1;
}

int main(void)
{
        struct k22info *buf;
        int num_entries = 100; // Start with a reasonable size
        int ret;
        int total_collected = 0;

        /* Allocate initial buffer */
        buf = malloc(num_entries * sizeof(struct k22info));
        if (!buf) {
                perror("malloc failed");
                return 1;
        }

        /* Keep calling the syscall and doubling buffer
	 * until we get all processes
	 */
        while (1) {
                int current_size = num_entries;
                int temp_entries = num_entries;

                printf("- User-space buf. size: %d\n", current_size);
                ret = syscall(467, buf, &temp_entries);
                printf("- syscall return val:   %d\n", ret);

                if (ret < 0) {
                        perror("syscall error");
                        free(buf);
                        return 1;
                }
                total_collected = temp_entries;
                /* If the number of entries equals what we requested,
                 * there might be more processes. Double the buffer and try again.
                 */
                if (total_collected >= current_size) {
				num_entries *= 2;
                        struct k22info *new_buf = realloc(buf, num_entries * sizeof(struct k22info));
                        if (!new_buf) {
                                perror("realloc failed");
                                free(buf);
                                return 1;
                        }
                        buf = new_buf;
                } else {
                        /* Either we got fewer entries than requested (all processes fit),
                         * or buffer size is already large enough
                         */
                        break;
                }
        }
        printf("--- OK ---\n\n");
        /* Allocate stack for tracking process hierarchy positions */
        int *stack = malloc(total_collected * sizeof(int));
        if (!stack) {
                perror("malloc failed for stack");
                free(buf);
                return 1;
        }
        /* Print header */
        printf("#comm,pid,ppid,fcldpid,nsblpid,nvcsw,nivcsw,stime\n");

        /* Print only the requested number of entries */
        if (total_collected > 0) {
                printf("%s,%d,%d,%d,%d,%ld,%ld,%ld\n",
                       buf[0].comm,
                       buf[0].pid,
                       buf[0].parent_pid,
                       buf[0].first_child_pid,
                       buf[0].next_sibling_pid,
                       buf[0].nvcsw,
                       buf[0].nivcsw,
                       buf[0].start_time);

                stack[0] = 0;  /* Root is at position 0 in the stack */
                int top = 0;

                /* Process remaining entries */
                for (int i = 1; i < total_collected; i++) {
                        struct k22info *p = &buf[i];

                        /* Find the position of the parent in the stack */
                        int parent_pos = find_parent(stack, top, p->parent_pid, buf);

                        if (parent_pos == -1) {
                                /* Parent not found (shouldn't happen with correct DFS) */
                                fprintf(stderr, "Warning: Parent PID %d not found for process %d\n",
                                        p->parent_pid, p->pid);
                                // parent_pos = top;  /* Default to current top */
                        }
                        /* Update stack: remove processes that are not ancestors of current */
                        top = parent_pos + 1;
                        /* Add current process to stack */
                        stack[top] = i;
                        /* Print dashes based on depth */
                        for (int j = 0; j < top; j++) {
                                printf("-");
                        }
                        /* Print process information */
                        printf("%s,%d,%d,%d,%d,%ld,%ld,%ld\n",
                               p->comm,
                               p->pid,
                               p->parent_pid,
                               p->first_child_pid,
                               p->next_sibling_pid,
                               p->nvcsw,
                               p->nivcsw,
                               p->start_time);
                }
        }
        /* Free allocated memory */
        free(stack);
        free(buf);
        return 0;
}
