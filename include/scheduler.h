#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SCHEDULER_STACK_SIZE 1024 * 1024

// Scheduler configuration type.
typedef struct scheduler_config_st {
  bool preemptive;
  double_t percentage_of_work_before_pause;
  uint64_t quantum_msec;
} scheduler_config_t;

// Scheduler function address type.
typedef void (*scheduler_f_addr_t)(void *f_arg, scheduler_config_t config);

// Scheduler callback function address type.
typedef void (*scheduler_cf_addr_t)(size_t id, void const *f_arg);

// Call once to initialize the scheduler.
void scheduler_init(scheduler_config_t config);

// Register an 'on start' callback.
void scheduler_on_start(scheduler_cf_addr_t cf_addr);

// Register an 'on continue' callback.
void scheduler_on_continue(scheduler_cf_addr_t cf_addr);

// Register an 'on pause' callback.
void scheduler_on_pause(scheduler_cf_addr_t cf_addr);

// Register an 'on end' callback.
void scheduler_on_end(scheduler_cf_addr_t cf_addr);

// Call to create a new task and add it to the list in a runnable state.
// Can be called outside of a task context or within a task context.
void scheduler_create_task(scheduler_f_addr_t f_addr, void *f_arg,
                           uint64_t ticket_n);

// Call to run the scheduler until all tasks are completed.
void scheduler_run(void);

// Call to exit the current task.
void scheduler_exit_current_task(void);

// Pause the task for a while.
// Returns when the scheduler decides.
void scheduler_pause_current_task(void);

#endif // !SCHEDULER_H
