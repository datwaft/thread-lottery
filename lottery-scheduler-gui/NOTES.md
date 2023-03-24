# Notes

Install GTK
https://www.ubuntubuzz.com/2018/11/setup-cgtk-programming-tools-on-ubuntu-for-beginners.html



Cool ideas for GUI.
- `modo de operacion` use `Combo Box Text`
- `Numero de threads` use `slider`? 

Read and write in Entry
```c
void on_button_execute_clicked(GtkWidget *widget, gpointer data) {
  gui_t *gui;
  gui = (gui_t *)data;

  g_print("lel\n");
  gtk_entry_set_text(GTK_ENTRY(gui->entry_thread_num), "dfdf");

  gchar *text;
  text = gtk_entry_get_text(GTK_ENTRY(gui->entry_thread_num));

  g_print("%s\n", text);
}
```

Display "enabled or disabled" button

```c
gtk_widget_set_sensitive(gui.button_execute, FALSE);
```

Tutorial
https://itstorage.net/index.php/lprogramm/cpgtkgladem/517-08gtkppc08

combo box
```c
  if (gtk_combo_box_get_active(GTK_COMBO_BOX(gui->cb_operation_mode)) != -1)
  {
    gchar *distro = gtk_combo_box_text_get_active_text(
        GTK_COMBO_BOX_TEXT(gui->cb_operation_mode));
    g_print("You chose %s\n", distro);
    g_free(distro);
  }
```