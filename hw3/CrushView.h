#include <gtk/gtk.h>
#ifndef _CView_h
#define _CView_h

void prepareView(int gridSquareDimension);

void directionPress(GtkWidget *widget, gpointer data);

void candySelect(GtkWidget *widget, gpointer data);

void g_application_open(GApplication *application, GFile **files, gint n_files, const gchar *hint);

#endif

