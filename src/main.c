#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef struct args_t {
  size_t id;
  // For calculations
  int64_t i;
  int64_t n;
  double_t result;
  double_t divisor;
  int8_t sign;
  // Configuration
  bool expropiative;
  double_t work_before_pause;
} args_t;

void calculate_pi(args_t *args) {
  for (; args->i < args->n; args->i += 1) {
    args->result += args->sign / args->divisor;
    args->divisor += 2;
    args->sign *= -1;

    if (!args->expropiative && args->i != 0 &&
        args->i % (int64_t)(args->n * args->work_before_pause) == 0) {
      scheduler_pause_current_task();
    }
  }
  args->result = 4 * args->result;
}

void on_pause(args_t *args) {
  int color = (args->id % (37 - 31)) + 31;
  printf("\x1b[2m"
         "Paused "
         "\x1b[22;%dm"
         "%zu"
         "\x1b[2;39m"
         ": current result is "
         "\x1b[22m"
         "%.10f"
         "\x1b[2m"
         " with "
         "\x1b[22m"
         "%.2f%%"
         "\x1b[2m"
         " of the work done."
         "\x1b[0m"
         "\n",
         color, args->id, 4 * args->result,
         (args->i / (double_t)args->n) * 100);
}

void on_end(args_t *args) {
  int color = (args->id % (37 - 31)) + 31;
  printf("\x1b[2m"
         "Finished "
         "\x1b[22;%dm"
         "%zu"
         "\x1b[2;39m"
         ": with "
         "\x1b[22m"
         "%lld"
         "\x1b[2m"
         " iterations, the value of PI is "
         "\x1b[22m"
         "%.10f"
         "\x1b[0m"
         "\n",
         color, args->id, args->n, args->result);
}

int main(int argc, char **argv) {
  bool expropiative = false;
  size_t thread_n = 5;
  int ticket_n = 5;
  int64_t work_n = 10000000;
  double_t work_before_pause = 0.05;

  scheduler_init();
  scheduler_on_pause((void (*)(void *))on_pause);
  scheduler_on_end((void (*)(void *))on_end);
  for (size_t id = 0; id < thread_n; ++id) {
    args_t *args = malloc(sizeof(args_t));
    args->id = id;

    args->i = 0;
    args->n = work_n * 50;
    args->result = 0;
    args->sign = 1;
    args->divisor = 1;

    args->expropiative = expropiative;
    args->work_before_pause = work_before_pause;

    scheduler_create_task((void (*)(void *))calculate_pi, args, ticket_n);
  }
  scheduler_run();
  printf("\x1b[1m"
         "Finished running all tasks!"
         "\x1b[0m"
         "\n");
  return EXIT_SUCCESS;
}
