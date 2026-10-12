#ifndef CALLBACK_H
#define CALLBACK_H

#include "app.h"

void child_exited(VteTerminal *terminal, gint status, gpointer data);
gboolean on_terminal_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
void on_termprop_changed(VteTerminal *terminal, const char *name, gpointer user_data);
void on_terminal_focus_changed(GObject *object, GParamSpec *pspec, gpointer data);
gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);

#endif // CALLBACK_H
