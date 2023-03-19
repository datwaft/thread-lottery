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
  for (size_t i = 1; i <= n; i++) {
    if (i == n) {
      printf("\x1b[9m"); // STRIKETHROUGH
    }
    printf("\x1b[2m" // DIM
           "task "
           "\x1b[22;%zum" // !DIM & color
           "%zu"
           "\x1b[39;2m" // !color & DIM
           ": "
           "\x1b[22m" // !DIM
           "%zu"
           "\x1b[2m" // DIM
           "/"
           "\x1b[22m" // !DIM
           "%zu"
           "\x1b[0m" // RESET
           "\n",
           31 + args->name, args->name, i, n);
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
  printf("\x1b[2m"
         "paused task "
         "\x1b[0;%zum"
         "%zu"
         "\x1b[0m"
         "\n",
         31 + args->name, args->name);
}

int main(int argc, char **argv) {
  scheduler_init();
  scheduler_on_pause((void (*)(void *))on_pause);
  for (size_t i = 0; i < 4; ++i) {
    create_test_task(i, pcg32_boundedrand(10 - 3) + 3);
  }
  scheduler_run();
  printf("Finished running all tasks!\n");
  return EXIT_SUCCESS;
}
