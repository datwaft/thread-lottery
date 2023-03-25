#include "gui.h"

void application_on_activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder = gtk_builder_new();
  if (!gtk_builder_add_from_file(builder, "template.glade", NULL)) {
    fprintf(stderr, "Couldn't read template, aborting.\n");
    abort();
  }
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gtk_builder_connect_signals(builder, NULL);
  gtk_widget_show_all(window);
  gtk_main();
}

void window_on_delete_event(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}
