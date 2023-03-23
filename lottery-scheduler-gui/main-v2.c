#include <gtk/gtk.h>
#include <stdio.h>

#define GLADE_FILE "glade/lottery-scheduler-gui-v2.glade"

int valor = 0;

typedef struct gui_t {
  float progress;
  GtkWidget *window_main;
  GtkWidget *button_execute;
  GtkWidget *spin_thread_num;
  GtkWidget *spin_unit;
  GtkWidget *cb_operation_mode;
  GtkWidget *label_quantum_or_percentage;
  GtkWidget *entry_quantum_or_percentage;
  GtkWidget *label_unit;
  GtkWidget *generic_progress_bar;
  GtkWidget *box_thread_config;

} gui_t;

static void activate(GtkApplication *app, gui_t gui, gpointer user_data);

void on_button_execute_clicked(GtkWidget *widget, gpointer data);

void on_changed(GtkComboBox *widget, gpointer data);

static void destroy(GtkWidget *widget, gpointer data);

void scheduler_stub();

/***********************************************************************************************************************/
static void activate(GtkApplication *app, gui_t gui, gpointer user_data) {
  GtkBuilder *builder;
  GtkWidget *window_main;

  gui.progress = 0.0;

  /* Import UI designed via Glade */
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

  window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gui.window_main = window_main;

  gui.button_execute =
      GTK_WIDGET(gtk_builder_get_object(builder, "button_execute"));

  gui.spin_thread_num =
      GTK_WIDGET(gtk_builder_get_object(builder, "spin_thread_num"));

  gui.spin_unit = GTK_WIDGET(gtk_builder_get_object(builder, "spin_unit"));

  gui.cb_operation_mode =
      GTK_WIDGET(gtk_builder_get_object(builder, "cb_operation_mode"));

  gui.label_unit = GTK_WIDGET(gtk_builder_get_object(builder, "label_unit"));

  gui.box_thread_config =
      GTK_WIDGET(gtk_builder_get_object(builder, "box_thread_config"));

  GtkWidget *label = gtk_label_new_with_mnemonic("_Hello");

  // gtk_box_set_child_packing(gui.box_thread_config, label, TRUE, TRUE, 0, );

  // for (int i = 0; i < 100; i++) {
  //   GtkWidget *button = gtk_button_new();
  //   button = gtk_button_new_with_label("SOY UN BOTON");
  //   gtk_box_pack_start(GTK_BOX(gui.box_thread_config), button, FALSE, FALSE,
  //   0); gtk_widget_show(button);
  // }

  int min_ticket = 5;
  int max_ticket = 100;

  int min_work = 200;
  int max_work = 1000;

  int threads_num = 100;

  for (int i = 0; i < threads_num; i++) {
    // create one grid_row per row, 3 columns
    GtkWidget *grid_row;
    grid_row = gtk_grid_new();


    char str_thread_number[5];
    sprintf(str_thread_number, "%d", i);

    GtkWidget *label_thread;
    label_thread = gtk_label_new(str_thread_number);

    GtkAdjustment *adjustment_ticket;
    adjustment_ticket = gtk_adjustment_new(min_ticket, min_ticket, max_ticket, 1.0, 5.0, 0.0);

    GtkAdjustment *adjustment_work;
    adjustment_work = gtk_adjustment_new(min_work, min_work, max_work, 1.0, 5.0, 0.0);

    GtkWidget *sbtn_ticket;
    sbtn_ticket = gtk_spin_button_new(adjustment_ticket, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_ticket), TRUE);

    GtkWidget *sbtn_work;
    sbtn_work = gtk_spin_button_new(adjustment_work, 1.0, 0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbtn_work), TRUE);

    // attach components to grid_row
    gtk_grid_attach(GTK_GRID(grid_row), label_thread, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_ticket, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_row), sbtn_work, 2, 0, 1, 1);
    gtk_widget_show(grid_row);


    gtk_grid_set_column_homogeneous(GTK_GRID(grid_row), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid_row), 3);

    gtk_box_pack_start(GTK_BOX(gui.box_thread_config), grid_row, FALSE, FALSE, 3);

    gtk_widget_show(sbtn_ticket);
    gtk_widget_show(sbtn_work);
    gtk_widget_show(label_thread);
  }

  /* Connects */
  gtk_builder_connect_signals(builder, NULL);

  g_signal_connect(window_main, "destroy", G_CALLBACK(destroy), &gui);

  g_signal_connect(gui.button_execute, "clicked",
                   G_CALLBACK(on_button_execute_clicked), &gui);

  g_object_unref(builder);
  gtk_widget_show(window_main);
  gtk_main();
}

void on_button_execute_clicked(GtkWidget *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;
  gui->progress += 0.1;

  gint val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_thread_num));

  // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gui->generic_progress_bar),
  //                               i);

  g_print("You chose %i\n", val);
}

void on_changed(GtkComboBox *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  // switch (gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cb_operation_mode))) {
  // case 0:
  //   g_print("You chose %s\n", "0");
  //   gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage),
  //                      "Tamaño Quantum");
  //   gtk_label_set_text(GTK_LABEL(gui->label_unit_quantum_or_percentage),
  //   "ms"); break;
  // case 1:
  //   g_print("You chose %s\n", "1");
  //   g_print("You chose %s\n", "0");
  //   gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage),
  //                      "Trabajo Mínimo");
  //   gtk_label_set_text(GTK_LABEL(gui->label_unit_quantum_or_percentage),
  //   "\%"); break;
  // default:
  //   g_print("You chose %s\n", "other");
  //   break;
  // }
}

static void destroy(GtkWidget *widget, gpointer data) { gtk_main_quit(); }

void scheduler_stub() {
  for (int i = 0; i < 10; i++) {
    g_print("%d\n", i);
    sleep(1);
  }
}

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
