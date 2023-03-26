#include "gui.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calculate_pi.h"
#include "deps/pcg_basic.h"
#include "scheduler.h"

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
  g_signal_connect(application, "activate", G_CALLBACK(application_on_activate),
                   NULL);
  return application;
}

void application_on_activate(GtkApplication *app, gpointer _) {
  user_data_t user_data = {
      .min_ticket = MIN_TICKET,
      .max_ticket = MAX_TICKET,
      .min_work = MIN_WORK,
      .max_work = MAX_WORK,
      .default_ticket = DEFAULT_TICKET,
      .default_work = DEFAULT_WORK,
      .default_thread_num = DEFAULT_THREAD_NUM,
      .operation_mode = INVALID,
  };

  GtkBuilder *builder = gtk_builder_new_from_resource(TEMPLATE_URI);
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

  user_data.button_execute =
      GTK_WIDGET(gtk_builder_get_object(builder, "button_execute"));

  user_data.spin_thread_num =
      GTK_WIDGET(gtk_builder_get_object(builder, "spin_thread_num"));

  user_data.spin_yield =
      GTK_WIDGET(gtk_builder_get_object(builder, "spin_yield"));

  user_data.cb_operation_mode =
      GTK_WIDGET(gtk_builder_get_object(builder, "cb_operation_mode"));

  user_data.label_quantum_or_percentage = GTK_WIDGET(
      gtk_builder_get_object(builder, "label_quantum_or_percentage"));

  user_data.box_thread_execution =
      GTK_WIDGET(gtk_builder_get_object(builder, "box_thread_execution"));

  user_data.box_thread_config =
      GTK_WIDGET(gtk_builder_get_object(builder, "box_thread_config"));

  user_data.adjustment_yield =
      GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adjustment_yield"));

  generate_thread_conf_row(user_data.default_thread_num, &user_data);

  gtk_builder_connect_signals(builder, &user_data);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data.spin_thread_num),
                            user_data.default_thread_num);

  gtk_widget_show_all(window);
  gtk_main();
}

void window_on_delete_event(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

GtkWidget *generate_thread_execution_row(int thread_id,
                                         user_data_t *user_data) {
  // create one grid_row per row, 3 columns
  GtkWidget *grid_row;
  grid_row = gtk_grid_new();

  char str_thread_number[5];
  sprintf(str_thread_number, "%d", thread_id);

  GtkWidget *label_thread_id;
  label_thread_id = gtk_label_new(str_thread_number);

  GtkWidget *progress_bar_thread;
  progress_bar_thread = gtk_progress_bar_new();
  gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar_thread), TRUE);

  GtkWidget *label_thread_result;
  label_thread_result = gtk_label_new("-");

  // attach components to grid_row: left for thread num, center for ticket
  // number, right for work
  gtk_grid_attach(GTK_GRID(grid_row), label_thread_id, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid_row), progress_bar_thread, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid_row), label_thread_result, 2, 0, 1, 1);
  gtk_widget_show(grid_row);

  gtk_grid_set_column_homogeneous(GTK_GRID(grid_row), TRUE);
  gtk_grid_set_column_spacing(GTK_GRID(grid_row), 3);

  gtk_box_pack_start(GTK_BOX(user_data->box_thread_execution), grid_row, FALSE,
                     FALSE, 3);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  gtk_widget_override_color(label_thread_id, GTK_STATE_FLAG_NORMAL,
                            &(GdkRGBA){.red = 1.0, .alpha = 1.0});
#pragma clang diagnostic pop

  gtk_widget_show(label_thread_id);
  gtk_widget_show(progress_bar_thread);
  gtk_widget_show(label_thread_result);

  return grid_row;
}

void generate_thread_conf_row(int threads_num, user_data_t *user_data) {
  for (int i = 0; i < threads_num; i++) {
    // create one grid_row per row, 3 columns
    GtkWidget *grid_row;
    grid_row = gtk_grid_new();

    char str_thread_number[5];
    sprintf(str_thread_number, "%d", i);

    GtkWidget *label_thread;
    label_thread = gtk_label_new(str_thread_number);

    GtkAdjustment *adjustment_ticket;
    adjustment_ticket =
        gtk_adjustment_new(user_data->default_ticket, user_data->min_ticket,
                           user_data->max_ticket, 1.0, 5.0, 0.0);

    GtkAdjustment *adjustment_work;
    adjustment_work =
        gtk_adjustment_new(user_data->default_work, user_data->min_work,
                           user_data->max_work, 1.0, 5.0, 0.0);

    GtkWidget *sbtn_ticket;
    sbtn_ticket = gtk_spin_button_new(adjustment_ticket, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_ticket), TRUE);

    GtkWidget *sbtn_work;
    sbtn_work = gtk_spin_button_new(adjustment_work, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_work), TRUE);

    // attach components to grid_row: left for thread num, center for ticket
    // number, right for work
    gtk_grid_attach(GTK_GRID(grid_row), label_thread, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_ticket, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_work, 2, 0, 1, 1);
    gtk_widget_show(grid_row);

    gtk_grid_set_column_homogeneous(GTK_GRID(grid_row), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid_row), 3);

    gtk_box_pack_start(GTK_BOX(user_data->box_thread_config), grid_row, FALSE,
                       FALSE, 3);

    gtk_widget_show(sbtn_ticket);
    gtk_widget_show(sbtn_work);
    gtk_widget_show(label_thread);
  }
}

void clear_thread_conf_row(user_data_t *user_data) {
  GList *children =
      gtk_container_get_children(GTK_CONTAINER(user_data->box_thread_config));

  while (children) {
    gtk_container_remove(GTK_CONTAINER(user_data->box_thread_config),
                         children->data);
    children = children->next;
  }
}

void clear_thread_execution_row(user_data_t *user_data) {
  GList *children = gtk_container_get_children(
      GTK_CONTAINER(user_data->box_thread_execution));

  while (children) {
    gtk_container_remove(GTK_CONTAINER(user_data->box_thread_execution),
                         children->data);
    children = children->next;
  }
}

void on_changed_sbtn_thread_num(GtkComboBox *widget, user_data_t *user_data) {
  gint threads_to_generate =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data->spin_thread_num));
  g_print("Change in thread to generate: %i\n", threads_to_generate);

  // remove all children
  clear_thread_conf_row(user_data);

  generate_thread_conf_row(threads_to_generate, user_data);
}

void on_button_execute_clicked(GtkWidget *widget, user_data_t *user_data) {
  clear_thread_execution_row(user_data);

  gint _cb_operation_mode =
      gtk_combo_box_get_active(GTK_COMBO_BOX(user_data->cb_operation_mode));
  gint _thread_num =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data->spin_thread_num));
  gint _yield =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data->spin_yield));

  g_print("\n --- Execution Configuration ---\n");
  g_print("Operation Mode: %i\n", _cb_operation_mode);
  g_print("Threads: %i\n", _thread_num);
  g_print("Yield: %i\n", _yield);

  int _thread = 0;
  GList *children =
      gtk_container_get_children(GTK_CONTAINER(user_data->box_thread_config));

  size_t thread_n = _thread_num;

  uint64_t ticket_n[thread_n];
  int64_t work_n[thread_n];

  // get generated values of the thread conf box
  while (children) {

    // we can start creating task here
    gint ticket_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 1, 0)));

    gint work_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 2, 0)));

    g_print("Thread: %i | Tickets: %i | Work: %i\n", _thread,
            ticket_column_value, work_column_value);

    ticket_n[_thread] = ticket_column_value;
    // work_n[_thread] = pow(10, _thread + 3);
    // work_n[_thread] = 1000000;
    work_n[_thread] = work_column_value;

    children = children->next;
    _thread++;
  }

  scheduler_config_t config = {.preemptive = !_cb_operation_mode,
                               .percentage_of_work_before_pause = _yield,
                               .quantum_msec = _yield};

  scheduler_init(config);
  scheduler_on_start((scheduler_cf_addr_t)on_start);
  scheduler_on_continue((scheduler_cf_addr_t)on_continue);
  scheduler_on_pause((scheduler_cf_addr_t)on_pause);
  scheduler_on_end((scheduler_cf_addr_t)on_end);

  for (size_t i = 0; i < thread_n; ++i) {
    args_t *args = malloc(sizeof(args_t));

    args->i = 0;
    args->n = work_n[i] * 50;
    args->result = 0;
    args->sign = 1;
    args->divisor = 1;
    args->row = generate_thread_execution_row(i, user_data);

    scheduler_create_task((scheduler_f_addr_t)calculate_pi, args, ticket_n[i]);
  }

  g_thread_new("run_scheduler_main_thread", (GThreadFunc)scheduler_run,
               user_data);

  g_print("\x1b[1m"
          "Finished running all tasks!"
          "\x1b[0m"
          "\n");

  g_print("end\n");
}

void on_sbutton_changed(GtkComboBox *widget, user_data_t *user_data) {
  gint active_selection =
      gtk_combo_box_get_active(GTK_COMBO_BOX(user_data->cb_operation_mode));

  switch (active_selection) {
  case PREEMPTIVE:
    g_print("Operation Mode: %d\n", PREEMPTIVE);
    gtk_label_set_text(GTK_LABEL(user_data->label_quantum_or_percentage),
                       "Tamaño Quantum(ms)");
    user_data->operation_mode = PREEMPTIVE;

    // enable button
    gtk_widget_set_sensitive(user_data->button_execute, TRUE);

    // reset to default values
    clear_thread_conf_row(user_data);

    generate_thread_conf_row(user_data->default_thread_num, user_data);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data->spin_thread_num),
                              user_data->default_thread_num);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data->spin_yield),
                              DEFAULT_PREEMPTIVE_YIELD);
    gtk_adjustment_set_lower(user_data->adjustment_yield, MIN_PREEMPTIVE_YIELD);
    gtk_adjustment_set_upper(user_data->adjustment_yield, MAX_PREEMPTIVE_YIELD);
    break;
  case NON_PREEMPTIVE:
    g_print("Operation Mode: %d\n", NON_PREEMPTIVE);
    gtk_label_set_text(GTK_LABEL(user_data->label_quantum_or_percentage),
                       "Trabajo Mínimo(%)");

    user_data->operation_mode = NON_PREEMPTIVE;

    // enable button
    gtk_widget_set_sensitive(user_data->button_execute, TRUE);

    // reset to default values
    clear_thread_conf_row(user_data);

    generate_thread_conf_row(user_data->default_thread_num, user_data);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data->spin_thread_num),
                              user_data->default_thread_num);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data->spin_yield),
                              DEFAULT_NON_PREEMPTIVE_YIELD);
    gtk_adjustment_set_lower(user_data->adjustment_yield,
                             MIN_NON_PREEMPTIVE_YIELD);
    gtk_adjustment_set_upper(user_data->adjustment_yield,
                             MAX_NON_PREEMPTIVE_YIELD);

    break;
  default:
    user_data->operation_mode = INVALID;
    g_print("Operation Mode: non selected %d\n", INVALID);
    break;
  }
}
