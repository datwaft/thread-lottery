#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define APPLICATION_ID "com.soa.lottery-scheduler"
#define TEMPLATE_URI "/com/soa/lottery-scheduler/template.ui"

GtkApplication *application_new(void);

void application_on_activate(GtkApplication *app, gpointer user_data);

void window_on_delete_event(GtkWidget *widget, gpointer user_data);

void generate_thread_conf_row(int threads_num, gpointer data);

void generate_thread_execution_row(int threads_num, gpointer data);

void clear_thread_conf_row(gpointer data);

void clear_thread_execution_row(gpointer data);

void on_changed_sbtn_thread_num(GtkComboBox *widget, gpointer data);

void on_button_execute_clicked(GtkWidget *widget, gpointer data);

void on_sbutton_changed(GtkComboBox *widget, gpointer data);

#endif // !GUI_H
