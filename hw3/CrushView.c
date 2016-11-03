#include <gtk/gtk.h>
#include "Array2D.h"
#include <stdlib.h>
#include "CrushModel.h"
#include "CrushView.h"

//char* locationOfSelected;
Point* locationOfSelected;
ModelData* md;
GtkWidget* grid;
GtkWidget* window;

void prepareView(int gridSquareDimension){

  GtkWidget *button;

  for (int i = 0; i <= md->arr->rows; i++) {
    for (int j = 0; j <= md->arr->columns; j++) {
      int* x = (int*) getArray2D(md->arr, j, i);
      char* disp = malloc(sizeof(char));
      sprintf(disp, "%d", *x);
      button = gtk_button_new_with_label(disp);
      g_signal_connect(button, "clicked", (GCallback) candySelect, (gpointer) getArray2D(md->points, j, i));
      gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * j, gridSquareDimension * i, gridSquareDimension, gridSquareDimension);
      free(disp);
    }
  }

  button = gtk_button_new_with_label("Left");
  g_signal_connect (button, "clicked", (GCallback) directionPress, (gpointer) "l"); //left
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * (md->arr->columns + 1) + 10, 60, 100, 60);
  button = gtk_button_new_with_label("Right");
  g_signal_connect (button, "clicked", (GCallback) directionPress, (gpointer) "r"); //right
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * (md->arr->columns + 1) + 10, 120, 100, 60);
  button = gtk_button_new_with_label("Up");
  g_signal_connect (button, "clicked", (GCallback) directionPress, (gpointer) "u"); //up
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * (md->arr->columns + 1) + 10, 180, 100, 60);
  button = gtk_button_new_with_label("Down");
  g_signal_connect (button, "clicked", (GCallback) directionPress, (gpointer) "d"); //down
  gtk_grid_attach(GTK_GRID(grid), button, gridSquareDimension * (md->arr->columns + 1) + 10, 240, 100, 60);

  char* moveText = (char*) malloc(sizeof(char) * 23);
  sprintf(moveText, "You have %d moves left", md->moves);
  GtkWidget* moveTextLabel = gtk_label_new(moveText);
  gtk_grid_attach(GTK_GRID(grid), moveTextLabel, gridSquareDimension * (md->arr->columns + 1) + 10, 0, 100, 60);
  free(moveText);
}

// is called when a directional button is pressed
void directionPress(GtkWidget *widget, gpointer data){
  int code;
  if (locationOfSelected != NULL){
    int x = locationOfSelected->x;
    int y = locationOfSelected->y;
    char dir = ((char*)data)[0];
    code = swapCandies(md, x, y, dir);
  }

  if (code == 0){ //swap success

    //delete the view components
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(grid));
    for(iter = children; iter != NULL; iter = g_list_next(iter)){
      gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    //redraw everything
    prepareView(50);
    gtk_widget_show_all(window);
  }
}

// is called when a candy button is pressed
void candySelect(GtkWidget *widget, gpointer data){
  locationOfSelected = (Point*) data;
}

//runs instead of activate when there's command line arguments
void g_application_open(GApplication *application, GFile **files, gint n_files,
			const gchar *hint){

  int gridSquareDimension = 50;

  /* create a new window, and set its title */
  window = gtk_application_window_new ((GtkApplication*) application);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new();

  /* Pack the container in the window */
  gtk_container_add(GTK_CONTAINER(window), grid);
  prepareView(gridSquareDimension);
  gtk_widget_show_all(window);
}

int main (int argc, char **argv){
  if (argc != 2){
      printf("Error: provide a single json file\n");
    return 1;
  }
  GtkApplication *app;
  int status;
  int* pointers;

  //set up the model
  Array2D arr = deseralizeInt2DArray(argv[1], &pointers);
  
  Array2D points = allocateArray2D(arr->rows + 1, arr->columns + 1);
  
  md = (ModelData*) malloc(sizeof(ModelData));
  md->arr = arr;
  md->points = points;
  md->moves = 30;

  for (int i = 0; i <= md->arr->rows; i++) {
    for (int j = 0; j <= md->arr->columns; j++) {
      Point* coords = malloc(sizeof(Point));
      coords->x = j;
      coords->y = i;
      setArray2D(md->points, coords, j, i);
    }
  }

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect (app, "open", G_CALLBACK (g_application_open), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  freeArray2D(arr, NULL);
  for (int i = 0; i <= md->arr->rows; i++) {
    for (int j = 0; j <= md->arr->columns; j++) {
      free(getArray2D(md->points, j, i));
    }
  }
  free(pointers);
  freeArray2D(points, NULL);
  free(md);
  return status;
  
}

