#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define APPLICATION_ID "com.soa.lottery-scheduler"
#define TEMPLATE_URI "/com/soa/lottery-scheduler/template.ui"

GtkApplication *application_new(void);

void application_on_activate(GtkApplication *app, gpointer user_data);

void window_on_delete_event(GtkWidget *widget, gpointer user_data);

#endif // !GUI_H
