#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

typedef struct {
  const char *name;
  size_t iters;
} args_t;

void tester(args_t *args) {
  for (size_t i = 0; i < args->iters; i++) {
    printf("task %s: %zu (%zu iterations)\n", args->name, i, args->iters);
    scheduler_pause_current_task();
  }
  free(args);
}

void create_test_task(const char *name, int iters) {
  args_t *args = malloc(sizeof(*args));
  args->name = name;
  args->iters = iters;
  scheduler_create_task((void (*)(void *))tester, args);
}

int main(int argc, char **argv) {
  scheduler_init();
  create_test_task("first", 5);
  create_test_task("second", 2);
  scheduler_run();
  printf("Finished running all tasks!\n");
  return EXIT_SUCCESS;
}
