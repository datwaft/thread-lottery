#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

typedef sigjmp_buf context_t;

typedef struct task_t {
  enum {
    TASK_CREATED,
    TASK_RUNNING,
  } status;

  size_t id;

  // This is where to jump back in order to resume the execution.
  context_t context;

  void (*function)(void *);
  void *args;

  void *stack_bottom;
  void *stack_top;
  size_t stack_size;
} task_t;

enum {
  SCHEDULER_INITIALIZED = 0,
  SCHEDULER_SCHEDULE,
  SCHEDULER_EXIT_TASK,
};

struct {
  // Where to jump back to perform scheduling.
  context_t context;
  // Current task
  task_t *current_task;
  // List of tasks
  size_t task_list_length;
  task_t **task_list;
} __scheduler;

static task_t *scheduler_choose_task(void);
static void schedule(void);
static void scheduler_free_current_task(void);
static void scheduler_free_task_list(void);

void scheduler_init(void) {
  __scheduler.current_task = NULL;
  __scheduler.task_list_length = 0;
  __scheduler.task_list = malloc(sizeof(task_t) * MAX_THREAD_N);
}

void scheduler_create_task(void (*function)(void *), void *args) {
  if (__scheduler.task_list_length >= MAX_THREAD_N) {
    fprintf(stderr,
            "Tried to create more tasks than the artificial maximum.\n");
    exit(EXIT_FAILURE);
  }

  // This value is saved between executions of this function.
  static size_t id = 1;

  task_t *task = malloc(sizeof(*task));
  task->status = TASK_CREATED;
  task->function = function;
  task->args = args;
  task->id = id;
  id += 1;
  task->stack_size = 16 * 1024; // This is fairly arbitrary.
  task->stack_bottom = malloc(task->stack_size);
  task->stack_top = (int8_t *)task->stack_bottom + task->stack_size;

  __scheduler.task_list[__scheduler.task_list_length] = task;
  __scheduler.task_list_length += 1;
}

void scheduler_exit_current_task(void) {
  task_t *task = __scheduler.current_task;

  // Remove task from task list
  // This algorithm just left shifts everything after the index.
  size_t index = -1;
  for (size_t i = 0; i < __scheduler.task_list_length; i += 1) {
    if (__scheduler.task_list[i] == task) {
      index = i;
      break;
    }
  }
  for (size_t i = index; i < __scheduler.task_list_length - 1; i += 1) {
    __scheduler.task_list[i] = __scheduler.task_list[i + 1];
  }
  __scheduler.task_list_length -= 1;

  // Go to the scheduler context.
  siglongjmp(__scheduler.context, SCHEDULER_EXIT_TASK);

  // NOTE: this function never returns.
  fprintf(stderr, "The function scheduler_exit_current_task() has returned.\n"
                  "It should never return.\n");
  exit(EXIT_FAILURE);
}

void scheduler_pause_current_task(void) {
  if (!sigsetjmp(__scheduler.current_task->context, false)) {
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
    asm volatile("mov %0, %%rsp" ::"rm"(next->stack_top));

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
  // Currently this is a round robin algorithm.
  // TODO: make it lottery scheduler.
  static size_t last_i = -1;
  if (__scheduler.task_list_length == 0) {
    return NULL;
  } else if (last_i + 1 >= __scheduler.task_list_length) {
    last_i = 0;
  } else {
    last_i += 1;
  }
  return __scheduler.task_list[last_i];
}

static void scheduler_free_current_task(void) {
  task_t *task = __scheduler.current_task;
  __scheduler.current_task = NULL;
  free(task->stack_bottom);
  free(task);
}

static void scheduler_free_task_list(void) {
  task_t **task_list = __scheduler.task_list;
  __scheduler.task_list = NULL;
  free(task_list);
}

// The code here was inspired by:
// https://brennan.io/2020/05/24/userspace-cooperative-multitasking/
