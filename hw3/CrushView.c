#include <gtk/gtk.h>
#include "Array2D.h"

Array2D arr;

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;

  int gridSquareDimension = 50;
  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add(GTK_CONTAINER(window), grid);

  for (int i = 0; i < arr->rows; i++) {
    for (int j = 0; j < arr->columns; j++) {
      int* x = (int*) getArray2D(arr, i, j);
      char* disp = malloc(sizeof(char));
      sprintf(disp, "%d", *x);
      button = gtk_button_new_with_label(disp);
      gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * i, gridSquareDimension * j, gridSquareDimension, gridSquareDimension);
      free(disp);
    }
  }

  button = gtk_button_new_with_label("Left");
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * arr->rows + 10, 0, 100, 60);
  button = gtk_button_new_with_label("Right");
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * arr->rows + 10,60, 100, 60);
  button = gtk_button_new_with_label("Up");
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * arr->rows + 10, 120, 100, 60);
  button = gtk_button_new_with_label("Down");
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * arr->rows + 10, 180, 100, 60);
  /* Now that we are done packing our widgets, we show them all
 *    * in one go, by calling gtk_widget_show_all() on the window.
 *       * This call recursively calls gtk_widget_show() on all widgets
 *          * that are contained in the window, directly or indirectly.
 *             */
  gtk_widget_show_all (window);

}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;
  int one = 1;
  int two = 2;
  int three = 3;
  int four = 4;
  int five = 5;

  srand(time(NULL));
  arr = allocateArray2D(5, 5);
  for (int i = 0; i < 4; i++) {
  for (int j = 0; j < 4; j++) {
    int x = rand() % 5;
    if (x == 0) {
      setArray2D(arr, &one, i, j);
    } else if (x == 1) {
      setArray2D(arr, &two, i, j);
    } else if (x == 2) {
      setArray2D(arr, &three, i, j);
    } else if (x == 3) {
      setArray2D(arr, &four, i, j);
    } else if (x == 4) {
      setArray2D(arr, &five, i, j);
    }
  }
  }

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  freeArray2D(arr, NULL);

  return status;
}

