#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "deps/pcg_basic.h"
#include "scheduler.h"

typedef struct args_st {
  int64_t i;
  int64_t n;
  double_t result;
  double_t divisor;
  int8_t sign;
} args_t;

void calculate_pi(args_t *args, scheduler_config_t config) {
  for (; args->i < args->n; args->i += 1) {
    args->result += args->sign / args->divisor;
    args->divisor += 2;
    args->sign *= -1;

    if (!config.preemptive && args->i != 0 &&
        args->i % (int64_t)(args->n * config.percentage_of_work_before_pause) ==
            0) {
      scheduler_pause_current_task();
    }
  }
  args->result = 4 * args->result;
}

void on_pause(size_t id, args_t *args) {
  int color = (id % (37 - 31)) + 31;
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
         color, id, 4 * args->result, (args->i / (double_t)args->n) * 100);
}

void on_end(size_t id, args_t *args) {
  int color = (id % (37 - 31)) + 31;
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
         color, id, args->n, args->result);
}

int main(int argc, char **argv) {
  size_t thread_n = 5;
  uint64_t ticket_n[thread_n];
  for (size_t i = 0; i < thread_n; i++) {
    ticket_n[i] = i + 1;
  }
  int64_t work_n[thread_n];
  for (size_t i = 0; i < thread_n; i++) {
    work_n[i] = pow(10, i + 4);
  }
  bool preemptive = true;
  double_t percentage_of_work_before_pause = 0.05;

  scheduler_config_t config = {.preemptive = preemptive,
                               .percentage_of_work_before_pause =
                                   percentage_of_work_before_pause};

  scheduler_init(config);
  scheduler_on_pause((scheduler_cf_addr_t)on_pause);
  scheduler_on_end((scheduler_cf_addr_t)on_end);
  for (size_t i = 0; i < thread_n; ++i) {
    args_t *args = malloc(sizeof(args_t));

    args->i = 0;
    args->n = work_n[i] * 50;
    args->result = 0;
    args->sign = 1;
    args->divisor = 1;

    scheduler_create_task((scheduler_f_addr_t)calculate_pi, args, ticket_n[i]);
  }
  scheduler_run();
  printf("\x1b[1m"
         "Finished running all tasks!"
         "\x1b[0m"
         "\n");
  return EXIT_SUCCESS;
}
