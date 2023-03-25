#include "gui.h"

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(application, "activate", G_CALLBACK(application_on_activate),
                   NULL);
  return application;
}

void application_on_activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder = gtk_builder_new();

  GError *error = NULL;
  if (!gtk_builder_add_from_resource(builder, TEMPLATE_URI, &error)) {
    g_printerr("%s\n", error->message);
    exit(EXIT_FAILURE);
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
