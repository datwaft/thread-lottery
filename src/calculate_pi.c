#include "calculate_pi.h"
#include <stdio.h>

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

void on_start(size_t id, args_t *args) {
  int color = (id % (37 - 31)) + 31;
  printf("\x1b[2m"
         "Started "
         "\x1b[22;%dm"
         "%zu"
         "\x1b[2m"
         "."
         "\x1b[0m"
         "\n",
         color, id);
}

void on_continue(size_t id, args_t *args) {
  int color = (id % (37 - 31)) + 31;
  printf("\x1b[2m"
         "Continued with "
         "\x1b[22;%dm"
         "%zu"
         "\x1b[2m"
         "."
         "\x1b[0m"
         "\n",
         color, id);
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
         "%ld"
         "\x1b[2m"
         " iterations, the value of PI is "
         "\x1b[22m"
         "%.10f"
         "\x1b[0m"
         "\n",
         color, id, args->n, args->result);
}