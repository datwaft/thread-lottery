#include "calculate_pi.h"
#include <stdio.h>

static void update_ui(args_t *args);

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
         color, id + 1);
  update_ui(args);
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
         color, id + 1);
  update_ui(args);
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
         color, id + 1, 4 * args->result, (args->i / (double_t)args->n) * 100);
  update_ui(args);
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
         color, id + 1, args->n, 4 * args->result);
  update_ui(args);
}

static void update_ui(args_t *args) {
  double progress = ((double)args->i / (double)args->n);

  char result_str[30]; // Probably won't require more than 30 digits.
  sprintf(result_str, "%0.20f", args->result * 4);
  gtk_label_set_text(
      GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(args->row), 2, 0)), result_str);

  gtk_progress_bar_set_fraction(
      GTK_PROGRESS_BAR(gtk_grid_get_child_at(GTK_GRID(args->row), 1, 0)),
      progress);

  if (progress >= 1.0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    gtk_widget_override_color(gtk_grid_get_child_at(GTK_GRID(args->row), 0, 0),
                              GTK_STATE_FLAG_NORMAL,
                              &(GdkRGBA){.green = 1.0, .alpha = 1.0});
#pragma clang diagnostic pop
  }
  while (g_main_context_pending(NULL)) {
    g_main_context_iteration(NULL, FALSE);
  }
}
