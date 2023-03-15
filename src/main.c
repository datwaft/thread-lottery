#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef struct args_t {
  size_t name;
  size_t iterations;
} args_t;

void tester(args_t *args) {
  for (size_t i = 0; i < args->iterations; i++) {
    if (i + 1 == args->iterations) {
      printf("\x1b[2;9m"
             "task "
             "\x1b[0;%lu;9m"
             "%zu"
             "\x1b[0;2;9m"
             ": "
             "\x1b[0;9m"
             "%zu"
             "\x1b[0;2;9m"
             "/"
             "\x1b[0;9m"
             "%zu"
             "\x1b[0m"
             "\n",
             31 + args->name, args->name, i + 1, args->iterations);
    } else {
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
             31 + args->name, args->name, i + 1, args->iterations);
    }
    scheduler_pause_current_task();
  }
  free(args);
}

void create_test_task(size_t name, int iters) {
  args_t *args = malloc(sizeof(*args));
  args->name = name;
  args->iterations = iters;
  scheduler_create_task((void (*)(void *))tester, args, 5);
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
