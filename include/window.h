#ifndef WINDOW_H
#define WINDOW_H

#include "app.h"

void add_css(void);
void create_tab(GtkWidget *widget, gpointer data);
void split_terminal(GtkNotebook *notebook, GtkOrientation orientation);
void activate(GtkApplication *app, gpointer user_data);

#endif // WINDOW_H
