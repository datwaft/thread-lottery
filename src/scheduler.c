#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/kvec.h"
#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef sigjmp_buf context_t;

typedef struct task_t {
  enum {
    TASK_CREATED,
    TASK_RUNNING,
  } status;

  size_t id;

  // For lottery scheduling.
  int ticket_n;

  // This is where to jump back in order to resume the execution.
  context_t context;

  void (*function)(void *);
  void *args;

  int8_t stack[SCHEDULER_STACK_SIZE];
} task_t;

enum {
  SCHEDULER_INITIALIZED = 0,
  SCHEDULER_SCHEDULE,
  SCHEDULER_EXIT_TASK,
};

struct {
  // Where to jump back to perform scheduling.
  context_t context;
  // Current task.
  task_t *current_task;
  // List of tasks.
  kvec_t(task_t *) tasks;
  // For lottery scheduling.
  int ticket_total;
  // Callbacks to call on events
  void (*on_pause)(void *);
} __scheduler;

static task_t *scheduler_choose_task(void);
static void schedule(void);
static void scheduler_free_current_task(void);
static void scheduler_free_task_list(void);

void scheduler_init(void) {
  __scheduler.current_task = NULL;
  kv_init(__scheduler.tasks);
}

void scheduler_on_pause(void (*callback)(void *)) {
  __scheduler.on_pause = callback;
}

void scheduler_create_task(void (*function)(void *), void *args, int ticket_n) {
  // This value is saved between executions of this function.
  static size_t id = 1;

  task_t *task = malloc(sizeof(*task));
  task->status = TASK_CREATED;
  task->function = function;
  task->args = args;
  task->id = id;
  id += 1;
  task->ticket_n = ticket_n;
  __scheduler.ticket_total += ticket_n;

  kv_push(task_t *, __scheduler.tasks, task);
}

void scheduler_exit_current_task(void) {
  task_t *current_task = __scheduler.current_task;

  // Find current task index in array
  int index = -1;
  for (size_t i = 0; i < kv_size(__scheduler.tasks); ++i) {
    if (kv_A(__scheduler.tasks, i) == current_task) {
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
  for (size_t i = index; i < kv_size(__scheduler.tasks); ++i) {
    kv_A(__scheduler.tasks, i) = kv_A(__scheduler.tasks, i + 1);
  }
  kv_pop(__scheduler.tasks);

  // Remove from the ticket total
  __scheduler.ticket_total -= current_task->ticket_n;

  // Go to the scheduler context.
  siglongjmp(__scheduler.context, SCHEDULER_EXIT_TASK);

  // NOTE: this function never returns.
  fprintf(stderr, "The function scheduler_exit_current_task() has returned.\n"
                  "It should never return.\n");
  exit(EXIT_FAILURE);
}

void scheduler_pause_current_task(void) {
  if (!sigsetjmp(__scheduler.current_task->context, false)) {
    // Execute the 'on pause' callback.
    if (__scheduler.on_pause) {
      __scheduler.on_pause(__scheduler.current_task->args);
    }
    siglongjmp(__scheduler.context, SCHEDULER_SCHEDULE);
  }
}

void scheduler_run(void) {
  switch (sigsetjmp(__scheduler.context, false)) {
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

static void schedule(void) {
  task_t *next = scheduler_choose_task();
  // This only happens if there are no tasks to schedule.
  if (next == NULL) {
    scheduler_free_task_list();
    // NOTE: this is the only case in which this function may return.
    return;
  }

  __scheduler.current_task = next;

  if (next->status == TASK_CREATED) {
    // Assign new stack.
    asm volatile("mov %0, %%rsp" ::"rm"(next->stack + SCHEDULER_STACK_SIZE));

    // Run the task function.
    next->status = TASK_RUNNING;
    next->function(next->args);

    // Exit the task.
    scheduler_exit_current_task();
  } else {
    // Go to the current task context.
    siglongjmp(next->context, true);
  }

  // NOTE: this function never returns implicitly.
  fprintf(stderr, "The function schedule() has returned implicitly.\n"
                  "It should never return implicitly.\n");
  exit(EXIT_FAILURE);
}

static task_t *scheduler_choose_task(void) {
  if (kv_size(__scheduler.tasks) == 0) {
    return NULL;
  }
  task_t **roulette = malloc(sizeof(task_t) * __scheduler.ticket_total);
  size_t roulette_size = 0;
  for (size_t i = 0; i < kv_size(__scheduler.tasks); ++i) {
    task_t *task = kv_A(__scheduler.tasks, i);
    for (int _ = 0; _ < task->ticket_n; _++) {
      roulette[roulette_size] = task;
      roulette_size += 1;
    }
  }

  if (roulette_size != (size_t)__scheduler.ticket_total) {
    fprintf(stderr, "Roulette size differs from the ticket total.\n"
                    "That this happened means that there is a bug.\n");
    exit(EXIT_FAILURE);
  }

  int winner_i = pcg32_boundedrand(roulette_size);
  task_t *winner = roulette[winner_i];
  free(roulette);
  return winner;
}

static void scheduler_free_current_task(void) {
  task_t *task = __scheduler.current_task;
  __scheduler.current_task = NULL;
  free(task);
}

static void scheduler_free_task_list(void) { kv_destroy(__scheduler.tasks); }

// The code here was inspired by:
// https://brennan.io/2020/05/24/userspace-cooperative-multitasking/
