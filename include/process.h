/*
 * process.h — Process Management & Scheduler
 * ============================================
 */

#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 16

typedef struct {
    int pid;
    char name[32];
    int active;
    int priority;
    void (*task_func)(void);
} Process;

/* Initialize the process table */
void process_init(void);

/* Create a new process. Returns PID or -1 on failure */
int process_create(const char *name, void (*task_func)(void));

/* Kill a process by PID */
int process_kill(int pid);

/* Run one tick of all active processes (round-robin) */
void process_schedule(void);

/* Print top-style process view */
void process_print_top(void);

/* Print simple ps output */
void process_print_ps(void);

/* Get number of active processes */
int process_get_count(void);

/* Built-in tasks */
void task_counter(void);
void task_snake(void); /* Foreground game */

#endif /* PROCESS_H */
