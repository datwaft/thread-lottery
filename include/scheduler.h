#ifndef SCHEDULER_H
#define SCHEDULER_H

// Artificial maximum of threads.
// WARNING: Cannot be too high as we are using an static array and
// initializing it from the start.
#define MAX_THREAD_N 1024

// Call once to initialize the scheduler.
void scheduler_init(void);

// Call to create a new task and add it to the list in a runnable state.
// Can be called outside of a task context or within a task context.
void scheduler_create_task(void (*function)(void *), void *args);

// Call to run the scheduler until all tasks are completed.
void scheduler_run(void);

// Call to exit the current task.
void scheduler_exit_current_task(void);

// Pause the task for a while.
// Returns when the scheduler decides.
void scheduler_pause_current_task(void);

#endif // !SCHEDULER_H
