#include <gtk/gtk.h>
#ifndef _CView_h
#define _CView_h

//to store coordinates associated with the buttons
typedef struct pointStruct {
    int x;
    int y;
} Point;

//Prepares the view to be displayed. int gridSquareDimension
//controls the size of the candies
void prepareView(int gridSquareDimension);

//Is called when a directional button is presssed.
//Uses the currently selected candy and the pressed direction
//to attempt a candy swap. If successful, the view will be redrawn.
//widget is the button being pressed. data contains a string
//representing that button's direction
void directionPress(GtkWidget *widget, gpointer data);

//Is called when a candy button is pressed. Changes the currently
//selected candy. widget is the candy being pressed. data contains
//a string representing the button's x, y location
void candySelect(GtkWidget *widget, gpointer data);

//Is called when serialize is pressed. Will serialize the current
//game to output.json
void serializeBoard(GtkWidget *widget, gpointer data);

//Is called after main when the program is run with a command line argument (which should be all
// the time). Sets up the window, view, and makes it visible.
void g_application_open(GApplication *application, GFile **files, gint n_files, const gchar *hint);

// Runs the game.
int runner(Array2D arr, Array2D state, int* moves, int* score, void (*instanceCaller)(int, int, int, int), const char* (*serializer)(char*), int argc, char** argv);

#endif /* _CView_h */

