#include <gtk/gtk.h>
#include <stdio.h>

#define GLADE_FILE "glade/lottery-scheduler-gui.glade"

typedef struct gui_t {
  GtkWidget *window_main;
  GtkWidget *button_execute;
  GtkWidget *entry_thread_num;
  GtkWidget *cb_operation_mode;
  GtkWidget *label_quantum_or_percentage;
  GtkWidget *entry_quantum_or_percentage;
  GtkWidget *label_unit_quantum_or_percentage;
} gui_t;

static void activate(GtkApplication *app, gui_t gui, gpointer user_data);

void on_button1_clicked(GtkWidget *widget, gpointer data);

void on_button_execute_clicked(GtkWidget *widget, gpointer data);

void on_changed(GtkComboBox *widget, gpointer data);

static void destroy(GtkWidget *widget, gpointer data);

/***********************************************************************************************************************/
static void activate(GtkApplication *app, gui_t gui, gpointer user_data) {
  GtkBuilder *builder;
  GtkWidget *window_main;

  /* Import UI designed via Glade */
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

  window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gui.window_main = window_main;

  gui.button_execute =
      GTK_WIDGET(gtk_builder_get_object(builder, "button_execute"));

  gui.entry_thread_num =
      GTK_WIDGET(gtk_builder_get_object(builder, "entry_thread_num"));

  gui.cb_operation_mode =
      GTK_WIDGET(gtk_builder_get_object(builder, "cb_operation_mode"));

  gui.label_quantum_or_percentage = GTK_WIDGET(
      gtk_builder_get_object(builder, "label_quantum_or_percentage"));

  gui.label_unit_quantum_or_percentage = GTK_WIDGET(
      gtk_builder_get_object(builder, "label_unit_quantum_or_percentage"));

  /* Connects */
  gtk_builder_connect_signals(builder, NULL);

  g_signal_connect(window_main, "destroy", G_CALLBACK(destroy), &gui);

  g_signal_connect(gui.button_execute, "clicked",
                   G_CALLBACK(on_button_execute_clicked), &gui);

  g_signal_connect(gui.cb_operation_mode, "changed", G_CALLBACK(on_changed),
                   &gui);

  g_object_unref(builder);
  gtk_widget_show(window_main);
  gtk_main();
}

void on_button_execute_clicked(GtkWidget *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  gchar *entry_text;
  entry_text = gtk_entry_get_text(GTK_ENTRY(gui->entry_thread_num));

  g_print("%s\n", entry_text);
}

void on_changed(GtkComboBox *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  switch (gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cb_operation_mode))) {
  case 0:
    g_print("You chose %s\n", "0");
    gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage), "Tamaño Quantum");
    gtk_label_set_text(GTK_LABEL(gui->label_unit_quantum_or_percentage), "ms");
    break;
  case 1:
    g_print("You chose %s\n", "1");
        g_print("You chose %s\n", "0");
    gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage), "Trabajo Mínimo");
    gtk_label_set_text(GTK_LABEL(gui->label_unit_quantum_or_percentage), "\%");
    break;
  default:
    g_print("You chose %s\n", "other");
    break;
  }
}

static void destroy(GtkWidget *widget, gpointer data) { gtk_main_quit(); }

int main(int argc, char **argv) {
  GtkApplication *app;
  int status = 0;
  /* Initialize the environment */
  app = gtk_application_new("com.soa.lottery.scheduler",
                            G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
