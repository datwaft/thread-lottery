#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

typedef sigjmp_buf context_t;

typedef struct {
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
  INIT = 0,
  SCHEDULE,
  EXIT_TASK,
};

struct {
  // Where to jump back to perform scheduling.
  context_t context;
  // Current task
  task_t *current_task;
  // List of tasks
  size_t task_n;
  task_t **tasks;
} scheduler_internal;

void scheduler_init(void) {
  scheduler_internal.current_task = NULL;
  scheduler_internal.task_n = 0;
  scheduler_internal.tasks = malloc(sizeof(task_t) * 1024);
}

void scheduler_create_task(void (*function)(void *), void *args) {
  static size_t id = 1;
  task_t *task = malloc(sizeof(*task));
  task->status = TASK_CREATED;
  task->function = function;
  task->args = args;
  task->id = id;
  id += 1;
  task->stack_size = 16 * 1024;
  task->stack_bottom = malloc(task->stack_size);
  task->stack_top = (int8_t *)task->stack_bottom + task->stack_size;

  scheduler_internal.tasks[scheduler_internal.task_n] = task;
  scheduler_internal.task_n += 1;
}

void scheduler_exit_current_task(void) {
  task_t *task = scheduler_internal.current_task;

  // Remove task from task list
  size_t index = -1;
  for (size_t i = 0; i < scheduler_internal.task_n; i += 1) {
    if (scheduler_internal.tasks[i]->id == task->id) {
      index = i;
      break;
    }
  }
  for (size_t i = index; i < scheduler_internal.task_n - 1; i += 1) {
    scheduler_internal.tasks[i] = scheduler_internal.tasks[i + 1];
  }
  scheduler_internal.task_n -= 1;

  siglongjmp(scheduler_internal.context, EXIT_TASK);

  // NOTE: This function never returns.
}

static task_t *scheduler_choose_task(void) {
  for (size_t i = 0; i < scheduler_internal.task_n; i += 1) {
    task_t *task = scheduler_internal.tasks[i];
    if (task->status == TASK_RUNNING || task->status == TASK_CREATED) {
      return task;
    }
  }
  return NULL;
}

static void schedule(void) {
  task_t *next = scheduler_choose_task();
  if (next == NULL) {
    return;
  }

  scheduler_internal.current_task = next;

  if (next->status == TASK_CREATED) {
    // Assign new stack
    register void *top = next->stack_top;
#ifdef __x86_64__
    __asm__ __volatile__("MOV %[rs], %%rsp \n" : [rs] "+r"(top)::);
#elif __arm__
    // TODO: check how to that assembly in ARM
    __asm__ __volatile__("MOV [rs], rsp \n" : [rs] "+r"(top)::);
#endif

    // Run the task function
    next->status = TASK_RUNNING;
    next->function(next->args);

    // WARNING: As the stack point is back where it was set returning would be a
    // VERY bad idea so it's better to just exit instead.
    scheduler_exit_current_task();
  } else {
    siglongjmp(next->context, true);
  }

  // NOTE: This function never returns.
}

void scheduler_pause_current_task(void) {
  if (!sigsetjmp(scheduler_internal.current_task->context, false)) {
    siglongjmp(scheduler_internal.context, SCHEDULE);
  }
}

static void scheduler_free_current_task(void) {
  task_t *task = scheduler_internal.current_task;
  scheduler_internal.current_task = NULL;
  free(task->stack_bottom);
  free(task);
}

void scheduler_run(void) {
  switch (sigsetjmp(scheduler_internal.context, false)) {
  case EXIT_TASK:
    scheduler_free_current_task();
  case INIT:
  case SCHEDULE:
    schedule();
    // Free list of tasks
    free(scheduler_internal.tasks);
    return;
  default:
    fprintf(stderr, "There has been an scheduler error: invalid state.\n");
    return;
  }
}
