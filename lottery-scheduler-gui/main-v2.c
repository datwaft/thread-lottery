#include <gtk/gtk.h>
#include <stdio.h>

#define GLADE_FILE "glade/lottery-scheduler-gui-v2.glade"
#define PREEMPTIVE 0
#define NON_PREEMPTIVE 1
#define PREEMPTIVE_YIELD 100
#define NON_PREEMPTIVE_YIELD 10
#define INVALID 999
#define MIN_TICKET 0
#define MAX_TICKET 1000
#define MIN_WORK 0
#define MAX_WORK 1000
#define DEFAULT_THREAD_NUM 5
#define DEFAULT_TICKET 5
#define DEFAULT_WORK 200

typedef struct gui_t {
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
  GtkWidget *view_thread_config;

} gui_t;

static void activate(GtkApplication *app, gui_t gui, gpointer user_data);

void on_button_execute_clicked(GtkWidget *widget, gpointer data);

void on_sbutton_changed(GtkComboBox *widget, gpointer data);

static void destroy(GtkWidget *widget, gpointer data);

void on_changed_sbtn_thread_num(GtkComboBox *widget, gpointer data);

void generate_thread_conf_row(int threads_num, gpointer data);

void clear_thread_conf_row(gpointer data);

static void activate(GtkApplication *app, gui_t gui, gpointer user_data) {
  GtkBuilder *builder;
  GtkWidget *window_main;

  // Default values
  gui.min_ticket = MIN_TICKET;
  gui.max_ticket = MAX_TICKET;

  gui.min_work = MIN_WORK;
  gui.max_work = MAX_WORK;

  gui.default_ticket = DEFAULT_TICKET;
  gui.default_work = DEFAULT_WORK;

  gui.default_thread_num = DEFAULT_THREAD_NUM;

  gui.operation_mode = INVALID;

  /* Import UI designed via Glade */
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, GLADE_FILE, NULL);

  window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gui.window_main = window_main;

  gui.button_execute =
      GTK_WIDGET(gtk_builder_get_object(builder, "button_execute"));

  gui.spin_thread_num =
      GTK_WIDGET(gtk_builder_get_object(builder, "spin_thread_num"));

  gui.spin_yield = GTK_WIDGET(gtk_builder_get_object(builder, "spin_yield"));

  gui.cb_operation_mode =
      GTK_WIDGET(gtk_builder_get_object(builder, "cb_operation_mode"));

  gui.label_quantum_or_percentage = GTK_WIDGET(
      gtk_builder_get_object(builder, "label_quantum_or_percentage"));

  gui.view_thread_config =
      GTK_WIDGET(gtk_builder_get_object(builder, "view_thread_config"));

  gui.box_thread_config =
      GTK_WIDGET(gtk_builder_get_object(builder, "box_thread_config"));

  generate_thread_conf_row(gui.default_thread_num, &gui);

  /* Connect components signal to functions */
  gtk_builder_connect_signals(builder, NULL);

  g_signal_connect(window_main, "destroy", G_CALLBACK(destroy), &gui);

  g_signal_connect(gui.button_execute, "clicked",
                   G_CALLBACK(on_button_execute_clicked), &gui);

  g_signal_connect(gui.cb_operation_mode, "changed",
                   G_CALLBACK(on_sbutton_changed), &gui);

  g_signal_connect(gui.spin_thread_num, "value-changed",
                   G_CALLBACK(on_changed_sbtn_thread_num), &gui);

  gtk_widget_set_sensitive(gui.button_execute, FALSE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.spin_thread_num),
                            gui.default_thread_num);

  g_object_unref(builder);
  gtk_widget_show(window_main);
  gtk_main();
}

void generate_thread_conf_row(int threads_num, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  for (int i = 0; i < threads_num; i++) {
    // create one grid_row per row, 3 columns
    GtkWidget *grid_row;
    grid_row = gtk_grid_new();

    char str_thread_number[5];
    sprintf(str_thread_number, "%d", i);

    GtkWidget *label_thread;
    label_thread = gtk_label_new(str_thread_number);

    GtkAdjustment *adjustment_ticket;
    adjustment_ticket = gtk_adjustment_new(gui->default_ticket, gui->min_ticket,
                                           gui->max_ticket, 1.0, 5.0, 0.0);

    GtkAdjustment *adjustment_work;
    adjustment_work = gtk_adjustment_new(gui->default_work, gui->min_work,
                                         gui->max_work, 1.0, 5.0, 0.0);

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

    gtk_box_pack_start(GTK_BOX(gui->box_thread_config), grid_row, FALSE, FALSE,
                       3);

    gtk_widget_show(sbtn_ticket);
    gtk_widget_show(sbtn_work);
    gtk_widget_show(label_thread);
  }
}

void clear_thread_conf_row(gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  GList *children =
      gtk_container_get_children(GTK_CONTAINER(gui->box_thread_config));

  while (children) {
    gtk_container_remove(GTK_CONTAINER(gui->box_thread_config), children->data);
    children = children->next;
  }
}

void on_changed_sbtn_thread_num(GtkComboBox *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  gint threads_to_generate =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_thread_num));
  g_print("Change in thread to generate: %i\n", threads_to_generate);

  // remove all children
  clear_thread_conf_row(data);

  generate_thread_conf_row(threads_to_generate, data);
}

void on_button_execute_clicked(GtkWidget *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  gint _cb_operation_mode =
      gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cb_operation_mode));
  gint _thread_num =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_thread_num));
  gint _yield = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spin_yield));

  g_print("\n --- Execution Configuration ---\n");
  g_print("Operation Mode: %i\n", _cb_operation_mode);
  g_print("Threads: %i\n", _thread_num);
  g_print("Yield: %i\n", _yield);

  int _thread = 0;
  GList *children =
      gtk_container_get_children(GTK_CONTAINER(gui->box_thread_config));

  // get generated values of the thread conf box
  while (children) {
    
    // we can start creating task here
    gint ticket_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 1, 0)));

    gint work_column_value = gtk_spin_button_get_value(
        GTK_SPIN_BUTTON(gtk_grid_get_child_at(GTK_GRID(children->data), 2, 0)));

    g_print("Thread: %i | Tickets: %i | Work: %i\n", _thread,
            ticket_column_value, work_column_value);

    children = children->next;
    _thread++;
  }
}

void on_sbutton_changed(GtkComboBox *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  gint active_selection =
      gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cb_operation_mode));

  switch (active_selection) {
  case PREEMPTIVE:
    g_print("Operation Mode: %d\n", PREEMPTIVE);
    gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage),
                       "Tamaño Quantum(ms)");
    gui->operation_mode = PREEMPTIVE;

    // enable button
    gtk_widget_set_sensitive(gui->button_execute, TRUE);

    // reset to default values
    clear_thread_conf_row(data);

    generate_thread_conf_row(gui->default_thread_num, data);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_thread_num),
                              gui->default_thread_num);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_yield),
                              PREEMPTIVE_YIELD);
    break;
  case NON_PREEMPTIVE:
    g_print("Operation Mode: %d\n", NON_PREEMPTIVE);
    gtk_label_set_text(GTK_LABEL(gui->label_quantum_or_percentage),
                       "Trabajo Mínimo(%)");

    gui->operation_mode = NON_PREEMPTIVE;

    // enable button
    gtk_widget_set_sensitive(gui->button_execute, TRUE);

    // reset to default values
    clear_thread_conf_row(data);

    generate_thread_conf_row(gui->default_thread_num, data);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_thread_num),
                              gui->default_thread_num);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spin_yield),
                              NON_PREEMPTIVE_YIELD);
    break;
  default:
    gui->operation_mode = INVALID;
    g_print("Operation Mode: non selected %d\n", INVALID);
    break;
  }
}

static void destroy(GtkWidget *widget, gpointer data) {
  g_print("Destroying all >:D\n");
  gtk_main_quit();
}

int main(int argc, char **argv) {
  GtkApplication *app;

  int status = 0;

  /* Initialize the environment */
  app = gtk_application_new("com.soa.lottery.scheduler",
                            G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app), argc, argv);

  g_print("Closing App\n");
  g_object_unref(app);

  return status;
}
