#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "deps/kvec.h"
#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef sigjmp_buf context_t;

typedef struct task_st {
  enum {
    TASK_CREATED,
    TASK_RUNNING,
  } status;
  context_t context;

  size_t id;

  uint64_t ticket_n;

  scheduler_f_addr_t f_addr;
  void *f_arg;

  int8_t stack[SCHEDULER_STACK_SIZE];
} task_t;

enum {
  SCHEDULER_INITIALIZED = 0,
  SCHEDULER_SCHEDULE,
  SCHEDULER_EXIT_TASK,
};

static struct {
  context_t context;
  scheduler_config_t config;

  size_t id;
  bool finished;

  task_t *current_task;
  kvec_t(task_t *) tasks;

  scheduler_cf_addr_t on_start;
  scheduler_cf_addr_t on_continue;
  scheduler_cf_addr_t on_pause;
  scheduler_cf_addr_t on_end;
} scheduler;

static task_t *scheduler_choose_task(void);
static void schedule(void);
static void scheduler_free_current_task(void);
static void scheduler_free_task_list(void);
static void scheduler_timer_callback(int signum);
static void scheduler_set_timer(void);

void scheduler_init(scheduler_config_t config) {
  scheduler.config = config;
  scheduler.id = 0;
  scheduler.finished = false;
  scheduler.current_task = NULL;
  kv_init(scheduler.tasks);
  signal(SIGALRM, scheduler_timer_callback);
}

void scheduler_on_start(scheduler_cf_addr_t cf_addr) {
  scheduler.on_start = cf_addr;
}

void scheduler_on_continue(scheduler_cf_addr_t cf_addr) {
  scheduler.on_continue = cf_addr;
}

void scheduler_on_pause(scheduler_cf_addr_t cf_addr) {
  scheduler.on_pause = cf_addr;
}

void scheduler_on_end(scheduler_cf_addr_t cf_addr) {
  scheduler.on_end = cf_addr;
}

void scheduler_create_task(scheduler_f_addr_t f_addr, void *f_arg,
                           uint64_t ticket_n) {
  task_t *task = malloc(sizeof(*task));
  task->status = TASK_CREATED;
  task->id = scheduler.id++;
  task->ticket_n = ticket_n;
  task->f_addr = f_addr;
  task->f_arg = f_arg;

  kv_push(task_t *, scheduler.tasks, task);
}

void scheduler_exit_current_task(void) {
  task_t *current_task = scheduler.current_task;

  // Execute 'on_end' callback
  if (scheduler.on_end) {
    scheduler.on_end(current_task->id, current_task->f_arg);
  }

  // Find current task index in array
  int64_t index = -1;
  for (size_t i = 0; i < kv_size(scheduler.tasks); ++i) {
    if (kv_A(scheduler.tasks, i) == current_task) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    fprintf(stderr, "The current task wasn't found in the task array.\n"
                    "This should never happen.\n");
    exit(EXIT_FAILURE);
  }

  // Shift every task after the index to the left and remove the last element.
  for (size_t i = index; i < kv_size(scheduler.tasks); ++i) {
    kv_A(scheduler.tasks, i) = kv_A(scheduler.tasks, i + 1);
  }
  kv_pop(scheduler.tasks);

  // Go to the scheduler context.
  siglongjmp(scheduler.context, SCHEDULER_EXIT_TASK);

  // NOTE: this function never returns.
  fprintf(stderr, "The function scheduler_exit_current_task() has returned.\n"
                  "It should never return.\n");
  exit(EXIT_FAILURE);
}

void scheduler_pause_current_task(void) {
  if (!sigsetjmp(scheduler.current_task->context, true)) {
    // Execute the 'on pause' callback.
    if (scheduler.on_pause) {
      scheduler.on_pause(scheduler.current_task->id,
                         scheduler.current_task->f_arg);
    }
    siglongjmp(scheduler.context, SCHEDULER_SCHEDULE);
  }
}

void scheduler_run(void) {
  switch (sigsetjmp(scheduler.context, true)) {
  case SCHEDULER_EXIT_TASK:
    scheduler_free_current_task();
    // NOTE: intentional passthrough.
  case SCHEDULER_INITIALIZED:
  case SCHEDULER_SCHEDULE:
    schedule();
    return;
  default:
    fprintf(stderr, "An invalid scheduler flag has been raised, aborting.\n");
    exit(EXIT_FAILURE);
  }
}

static void scheduler_timer_callback(int signum) {
  if (!scheduler.finished) {
    scheduler_pause_current_task();
  }
}

static void scheduler_set_timer(void) {
  ualarm(scheduler.config.quantum_msec * 1000, 0);
}

static void schedule(void) {
  task_t *next = scheduler_choose_task();
  // This only happens if there are no tasks to schedule.
  if (next == NULL) {
    scheduler_free_task_list();
    scheduler.finished = true;
    // NOTE: this is the only case in which this function may return.
    return;
  }

  scheduler.current_task = next;

  if (scheduler.config.preemptive) {
    scheduler_set_timer();
  }

  if (next->status == TASK_CREATED) {
    // Execute 'on_start' callback
    if (scheduler.on_start) {
      scheduler.on_start(next->id, next->f_arg);
    }

    // Assign new stack.
    asm volatile("mov %0, %%rsp" ::"rm"(next->stack + SCHEDULER_STACK_SIZE));
    // Run the task function.
    next->status = TASK_RUNNING;
    next->f_addr(next->f_arg, scheduler.config);

    // Exit the task.
    scheduler_exit_current_task();
  } else {
    // Execute 'on_continue' callback
    if (scheduler.on_continue) {
      scheduler.on_continue(next->id, next->f_arg);
    }
    // Go to the current task context.
    siglongjmp(next->context, true);
  }

  // NOTE: this function never returns implicitly.
  fprintf(stderr, "The function schedule() has returned implicitly.\n"
                  "It should never return implicitly.\n");
  exit(EXIT_FAILURE);
}

static task_t *scheduler_choose_task(void) {
  if (kv_size(scheduler.tasks) == 0) {
    return NULL;
  }

  // Calculate total number of tickets
  uint64_t ticket_total = 0;
  for (size_t i = 0; i < kv_size(scheduler.tasks); ++i) {
    ticket_total += kv_A(scheduler.tasks, i)->ticket_n;
  }

  // Create roulette
  task_t *roulette[ticket_total];
  size_t roulette_size = 0;
  for (size_t i = 0; i < kv_size(scheduler.tasks); ++i) {
    task_t *task = kv_A(scheduler.tasks, i);
    for (uint64_t _ = 0; _ < task->ticket_n; _++) {
      roulette[roulette_size++] = task;
    }
  }

  if (roulette_size != ticket_total) {
    fprintf(stderr, "Roulette size differs from the ticket total.\n"
                    "That this happened means that there is a bug.\n");
    exit(EXIT_FAILURE);
  }

  return roulette[pcg32_boundedrand(roulette_size)];
}

static void scheduler_free_current_task(void) {
  task_t *task = scheduler.current_task;
  scheduler.current_task = NULL;
  free(task);
}

static void scheduler_free_task_list(void) {
  kv_destroy(scheduler.tasks);
  kv_init(scheduler.tasks);
}

// The code here was inspired by:
// https://brennan.io/2020/05/24/userspace-cooperative-multitasking/
