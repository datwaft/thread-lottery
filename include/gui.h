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
  GtkBuilder *builder;
  int operation_mode;
} user_data_t;

GtkApplication *application_new(void);

void application_on_activate(GtkApplication *app, gpointer user_data);

void window_on_delete_event(GtkWidget *widget, gpointer user_data);

void on_changed_sbtn_thread_num(GtkComboBox *widget, user_data_t *user_data);

void on_button_execute_clicked(GtkWidget *widget, user_data_t *user_data);

void on_mode_change(GtkComboBox *widget, user_data_t *user_data);

#endif // !GUI_H
