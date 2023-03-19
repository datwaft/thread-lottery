#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef struct args_t {
  size_t name;
  size_t iterations;
} args_t;

void tester(args_t *args) {
  size_t n = args->iterations;
  // red is 31
  // ...
  // white is 37
  int color = (args->name % (37 - 31)) + 31;
  for (size_t i = 1; i <= n; i++) {
    if (i == n) {
      printf("\x1b[9m"); // STRIKETHROUGH
    }
    printf("\x1b[2m" // DIM
           "task "
           "\x1b[22;%dm" // !DIM & color
           "%zu"
           "\x1b[39;2m" // !color & DIM
           ": "
           "\x1b[22m" // !DIM
           "%zu"
           "\x1b[2m" // DIM
           "/"
           "\x1b[22m" // !DIM
           "%zu"
           "\x1b[0m" // DEFAULT
           "\n",
           color, args->name, i, n);
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

void on_pause(args_t *args) {
  int color = (args->name % (37 - 31)) + 31;
  printf("\x1b[2m" // DIM
         "paused task "
         "\x1b[22;%dm" // !DIM & color
         "%zu"
         "\x1b[0m" // DEFAULT
         "\n",
         color, args->name);
}

int main(int argc, char **argv) {
  scheduler_init();
  scheduler_on_pause((void (*)(void *))on_pause);
  for (size_t i = 0; i < 4; ++i) {
    create_test_task(i, pcg32_boundedrand(10 - 3) + 3);
  }
  scheduler_run();
  printf("\x1b[1m"
         "Finished running all tasks!"
         "\x1b[0m"
         "\n");
  return EXIT_SUCCESS;
}
