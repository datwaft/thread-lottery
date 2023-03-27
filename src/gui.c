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

static gpointer execute_scheduler(user_data_t *user_data);
static void disable_widgets(user_data_t *user_data);
static void enable_widgets(user_data_t *user_data);
static void reset_parameters(user_data_t *user_data, bool preemptive);
static void generate_configuration_rows(int row_n, user_data_t *user_data);
static GtkWidget *generate_execution_row(int row, user_data_t *user_data);
static void clear_configuration_rows(user_data_t *user_data);
static void clear_execution_rows(user_data_t *user_data);

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
  g_signal_connect(application, "activate", G_CALLBACK(application_on_activate),
                   NULL);
  return application;
}

void application_on_activate(GtkApplication *app, gpointer _) {
  user_data_t user_data = {
      .builder = gtk_builder_new_from_resource(TEMPLATE_URI),
  };
  gtk_builder_connect_signals(user_data.builder, &user_data);

  generate_configuration_rows(DEFAULT_THREAD_NUM, &user_data);

  GtkSpinButton *spin_thread_num = GTK_SPIN_BUTTON(
      gtk_builder_get_object(user_data.builder, "spin_thread_num"));
  gtk_spin_button_set_value(spin_thread_num, DEFAULT_THREAD_NUM);

  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(user_data.builder, "window_main"));
  gtk_widget_show_all(window);

  gtk_main();
}

void window_on_delete_event(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

void on_changed_sbtn_thread_num(GtkComboBox *widget, user_data_t *user_data) {
  GtkSpinButton *spin_thread_num = GTK_SPIN_BUTTON(
      gtk_builder_get_object(user_data->builder, "spin_thread_num"));
  gint new_rows = gtk_spin_button_get_value(spin_thread_num);

  clear_configuration_rows(user_data);
  generate_configuration_rows(new_rows, user_data);
}

void on_button_execute_clicked(GtkWidget *widget, user_data_t *user_data) {
  clear_execution_rows(user_data);

  GtkComboBox *cb_operation_mode = GTK_COMBO_BOX(
      gtk_builder_get_object(user_data->builder, "cb_operation_mode"));
  GtkSpinButton *spin_thread_num = GTK_SPIN_BUTTON(
      gtk_builder_get_object(user_data->builder, "spin_thread_num"));
  GtkSpinButton *spin_yield =
      GTK_SPIN_BUTTON(gtk_builder_get_object(user_data->builder, "spin_yield"));

  bool preemptive = !gtk_combo_box_get_active(cb_operation_mode);
  gint task_n = gtk_spin_button_get_value(spin_thread_num);
  gint yield = gtk_spin_button_get_value(spin_yield);

  GtkContainer *box_thread_config = GTK_CONTAINER(
      gtk_builder_get_object(user_data->builder, "box_thread_config"));
  GList *children = gtk_container_get_children(box_thread_config);

  uint64_t ticket_n[task_n];
  int64_t work_n[task_n];

  for (gint i = 0; children; ++i, children = children->next) {
    gint ticket_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 1, 0)));
    gint work_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 2, 0)));
    ticket_n[i] = ticket_column_value;
    work_n[i] = work_column_value;
  }

  scheduler_config_t config = {.preemptive = preemptive,
                               .percentage_of_work_before_pause =
                                   (double)yield / 100,
                               .quantum_msec = yield};

  scheduler_init(config);
  scheduler_on_start((scheduler_cf_addr_t)on_start);
  scheduler_on_continue((scheduler_cf_addr_t)on_continue);
  scheduler_on_pause((scheduler_cf_addr_t)on_pause);
  scheduler_on_end((scheduler_cf_addr_t)on_end);

  for (gint i = 0; i < task_n; ++i) {
    args_t *args = malloc(sizeof(args_t));

    args->i = 0;
    args->n = work_n[i] * 50;
    args->result = 0;
    args->sign = 1;
    args->divisor = 1;
    args->row = generate_execution_row(i, user_data);

    scheduler_create_task((scheduler_f_addr_t)calculate_pi, args, ticket_n[i]);
  }

  g_thread_new("run_scheduler_main_thread", (GThreadFunc)execute_scheduler,
               user_data);
}

void on_mode_change(GtkComboBox *widget, user_data_t *user_data) {
  GtkComboBox *cb_operation_mode = GTK_COMBO_BOX(
      gtk_builder_get_object(user_data->builder, "cb_operation_mode"));
  GtkWidget *button_execute =
      GTK_WIDGET(gtk_builder_get_object(user_data->builder, "button_execute"));
  GtkLabel *label_quantum_or_percentage = GTK_LABEL(gtk_builder_get_object(
      user_data->builder, "label_quantum_or_percentage"));

  gint mode = gtk_combo_box_get_active(cb_operation_mode);
  gtk_widget_set_sensitive(button_execute, TRUE);

  switch (mode) {
  case PREEMPTIVE:
    gtk_label_set_text(label_quantum_or_percentage, "Tamaño Quantum(ms)");
    reset_parameters(user_data, true);
    break;
  case NON_PREEMPTIVE:
    gtk_label_set_text(label_quantum_or_percentage, "Trabajo Mínimo(%)");
    reset_parameters(user_data, false);
    break;
  }
}

static gpointer execute_scheduler(user_data_t *user_data) {
  g_idle_add((GSourceFunc)disable_widgets, user_data);
  scheduler_run();
  g_idle_add((GSourceFunc)enable_widgets, user_data);
  return NULL;
}

static void disable_widgets(user_data_t *user_data) {
  GtkWidget *button_execute =
      GTK_WIDGET(gtk_builder_get_object(user_data->builder, "button_execute"));
  GtkWidget *cb_operation_mode = GTK_WIDGET(
      gtk_builder_get_object(user_data->builder, "cb_operation_mode"));
  gtk_widget_set_sensitive(button_execute, false);
  gtk_widget_set_sensitive(cb_operation_mode, false);
}

static void enable_widgets(user_data_t *user_data) {
  GtkWidget *button_execute =
      GTK_WIDGET(gtk_builder_get_object(user_data->builder, "button_execute"));
  GtkWidget *cb_operation_mode = GTK_WIDGET(
      gtk_builder_get_object(user_data->builder, "cb_operation_mode"));
  gtk_widget_set_sensitive(button_execute, true);
  gtk_widget_set_sensitive(cb_operation_mode, true);
}

static void clear_configuration_rows(user_data_t *user_data) {
  GtkContainer *box_thread_config = GTK_CONTAINER(
      gtk_builder_get_object(user_data->builder, "box_thread_config"));
  GList *children = gtk_container_get_children(box_thread_config);
  while (children) {
    gtk_container_remove(box_thread_config, children->data);
    children = children->next;
  }
}

static void clear_execution_rows(user_data_t *user_data) {
  GtkContainer *box_thread_execution = GTK_CONTAINER(
      gtk_builder_get_object(user_data->builder, "box_thread_execution"));
  GList *children = gtk_container_get_children(box_thread_execution);
  while (children) {
    gtk_container_remove(box_thread_execution, children->data);
    children = children->next;
  }
}

static GtkWidget *generate_execution_row(int row, user_data_t *user_data) {
  GtkWidget *grid_row = gtk_grid_new();

  char row_str[5]; // We probably won't need more than 5 digits.
  sprintf(row_str, "%d", row + 1);
  GtkWidget *label_row = gtk_label_new(row_str);

  GtkWidget *progress_bar = gtk_progress_bar_new();
  gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);

  GtkWidget *label_result = gtk_label_new("-");

  gtk_grid_attach(GTK_GRID(grid_row), label_row, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid_row), progress_bar, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid_row), label_result, 2, 0, 1, 1);
  gtk_widget_show(grid_row);

  gtk_grid_set_column_homogeneous(GTK_GRID(grid_row), TRUE);
  gtk_grid_set_column_spacing(GTK_GRID(grid_row), 3);

  GtkBox *box_thread_execution = GTK_BOX(
      gtk_builder_get_object(user_data->builder, "box_thread_execution"));
  gtk_box_pack_start(box_thread_execution, grid_row, FALSE, FALSE, 3);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  gtk_widget_override_color(label_row, GTK_STATE_FLAG_NORMAL,
                            &(GdkRGBA){.red = 1.0, .alpha = 1.0});
#pragma clang diagnostic pop

  gtk_widget_show(label_row);
  gtk_widget_show(progress_bar);
  gtk_widget_show(label_result);

  return grid_row;
}

static void generate_configuration_rows(int row_n, user_data_t *user_data) {
  for (int row = 0; row < row_n; row++) {
    GtkWidget *grid_row = gtk_grid_new();

    // We start at enumerating rows at 1.
    char row_str[5]; // We probably won't need more than 5 digits.
    sprintf(row_str, "%d", row + 1);
    GtkWidget *label_row = gtk_label_new(row_str);

    GtkAdjustment *adjustment_ticket = gtk_adjustment_new(
        DEFAULT_TICKET, MIN_TICKET, MAX_TICKET, 1.0, 5.0, 0.0);
    GtkWidget *sbtn_ticket = gtk_spin_button_new(adjustment_ticket, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_ticket), true);

    GtkAdjustment *adjustment_work =
        gtk_adjustment_new(DEFAULT_WORK, MIN_WORK, MAX_WORK, 1.0, 5.0, 0.0);
    GtkWidget *sbtn_work = gtk_spin_button_new(adjustment_work, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_work), true);

    gtk_grid_attach(GTK_GRID(grid_row), label_row, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_ticket, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_work, 2, 0, 1, 1);
    gtk_widget_show(grid_row);

    gtk_grid_set_column_homogeneous(GTK_GRID(grid_row), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid_row), 3);

    GtkBox *box_thread_config = GTK_BOX(
        gtk_builder_get_object(user_data->builder, "box_thread_config"));
    gtk_box_pack_start(box_thread_config, grid_row, FALSE, FALSE, 3);

    gtk_widget_show(sbtn_ticket);
    gtk_widget_show(sbtn_work);
    gtk_widget_show(label_row);
  }
}

static void reset_parameters(user_data_t *user_data, bool preemptive) {
  clear_configuration_rows(user_data);
  generate_configuration_rows(DEFAULT_THREAD_NUM, user_data);

  GtkSpinButton *spin_thread_num = GTK_SPIN_BUTTON(
      gtk_builder_get_object(user_data->builder, "spin_thread_num"));
  GtkSpinButton *spin_yield =
      GTK_SPIN_BUTTON(gtk_builder_get_object(user_data->builder, "spin_yield"));
  GtkAdjustment *adjustment_yield = GTK_ADJUSTMENT(
      gtk_builder_get_object(user_data->builder, "adjustment_yield"));

  // Make thread_n the default value.
  gtk_spin_button_set_value(spin_thread_num, DEFAULT_THREAD_NUM);
  // Make yield the default value.
  if (preemptive) {
    gtk_adjustment_set_lower(adjustment_yield, MIN_PREEMPTIVE_YIELD);
    gtk_adjustment_set_upper(adjustment_yield, MAX_PREEMPTIVE_YIELD);
    gtk_spin_button_set_value(spin_yield, DEFAULT_PREEMPTIVE_YIELD);
  } else {
    gtk_adjustment_set_lower(adjustment_yield, MIN_NON_PREEMPTIVE_YIELD);
    gtk_adjustment_set_upper(adjustment_yield, MAX_NON_PREEMPTIVE_YIELD);
    gtk_spin_button_set_value(spin_yield, DEFAULT_NON_PREEMPTIVE_YIELD);
  }
}
