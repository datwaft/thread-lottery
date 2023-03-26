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

void update_ui(GtkWidget *widget, double result, double progress_percent) {
  // result label
  char result_str[30];
  sprintf(result_str, "%0.20f", result);
  gtk_label_set_text(GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(widget), 2, 0)),
                     result_str);

  // progress bar
  gtk_progress_bar_set_fraction(
      GTK_PROGRESS_BAR(gtk_grid_get_child_at(GTK_GRID(widget), 1, 0)),
      progress_percent);

  // done result coloring
  if (progress_percent >= 1.0) {
    const static GdkRGBA green = {
        .red = 0.0, .green = 1.0, .blue = 0.0, .alpha = 1.0};
    gtk_widget_override_color(
        GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(widget), 0, 0)),
        GTK_STATE_FLAG_NORMAL, &green);
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
         color, id);
  update_ui(args->row, args->result * 4, (args->i / (double_t)args->n));
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
  update_ui(args->row, args->result * 4, (args->i / (double_t)args->n));
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
  update_ui(args->row, args->result * 4, (args->i / (double_t)args->n));
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
  update_ui(args->row, args->result, (args->i / (double_t)args->n));
}
