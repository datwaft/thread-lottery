#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef struct {
  size_t name;
  size_t iters;
} args_t;

void tester(args_t *args) {
  for (size_t i = 0; i < args->iters; i++) {
    printf("\x1b[2m"
           "task "
           "\x1b[0;%lum"
           "%zu"
           "\x1b[0;2m"
           ": "
           "\x1b[0m"
           "%zu"
           "\x1b[0;2m"
           "/"
           "\x1b[0m"
           "%zu"
           "\x1b[0m"
           "\n",
           31 + args->name, args->name, i + 1, args->iters);
    scheduler_pause_current_task();
  }
  free(args);
}

void create_test_task(size_t name, int iters) {
  args_t *args = malloc(sizeof(*args));
  args->name = name;
  args->iters = iters;
  scheduler_create_task((void (*)(void *))tester, args);
}

int main(int argc, char **argv) {
  scheduler_init();
  for (size_t i = 0; i < 4; ++i) {
    create_test_task(i, pcg32_boundedrand(10 - 3) + 3);
  }
  scheduler_run();
  printf("Finished running all tasks!\n");
  return EXIT_SUCCESS;
}
