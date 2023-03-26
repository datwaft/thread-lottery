#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

// Application constants
#define APPLICATION_ID "com.soa.lottery-scheduler"
#define TEMPLATE_URI "/com/soa/lottery-scheduler/template.ui"
// Value constants
#define PREEMPTIVE 0
#define NON_PREEMPTIVE 1
#define INVALID 999
// Minimum and maximum value constants
#define MIN_TICKET 1
#define MAX_TICKET 1000
#define MIN_WORK 1
#define MAX_WORK 10000000
#define MIN_PREEMPTIVE_YIELD 100
#define MAX_PREEMPTIVE_YIELD 10000
#define MIN_NON_PREEMPTIVE_YIELD 1
#define MAX_NON_PREEMPTIVE_YIELD 100
// Default value constants
#define DEFAULT_THREAD_NUM 5
#define DEFAULT_TICKET 5
#define DEFAULT_WORK 100000
#define DEFAULT_PREEMPTIVE_YIELD 100
#define DEFAULT_NON_PREEMPTIVE_YIELD 10

typedef struct user_data_st {
  float progress;
  int min_ticket;
  int max_ticket;
  int min_work;
  int max_work;
  int default_ticket;
  int default_work;
  int default_thread_num;
  int operation_mode;
  GtkWidget *window_main;
  GtkWidget *button_execute;
  GtkWidget *spin_thread_num;
  GtkWidget *spin_yield;
  GtkWidget *cb_operation_mode;
  GtkWidget *label_quantum_or_percentage;
  GtkWidget *entry_quantum_or_percentage;
  GtkWidget *label_unit;
  GtkWidget *generic_progress_bar;
  GtkWidget *box_thread_config;
  GtkWidget *box_thread_execution;
  GtkAdjustment *adjustment_yield;
} user_data_t;

GtkApplication *application_new(void);

void application_on_activate(GtkApplication *app, gpointer user_data);

void window_on_delete_event(GtkWidget *widget, gpointer user_data);

void generate_thread_conf_row(int threads_num, user_data_t *user_data);

GtkWidget *generate_thread_execution_row(int threads_num,
                                         user_data_t *user_data);

void clear_thread_conf_row(user_data_t *user_data);

void clear_thread_execution_row(user_data_t *user_data);

void on_changed_sbtn_thread_num(GtkComboBox *widget, user_data_t *user_data);

void on_button_execute_clicked(GtkWidget *widget, user_data_t *user_data);

void on_sbutton_changed(GtkComboBox *widget, user_data_t *user_data);

#endif // !GUI_H
