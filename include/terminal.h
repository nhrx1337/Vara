#ifndef TERMINAL_H
#define TERMINAL_H

#include "app.h"

void increase_font(VteTerminal *terminal);
void decrease_font(VteTerminal *terminal);
void reset_font(VteTerminal *terminal);
void set_terminal_colors(VteTerminal *terminal);
void set_terminal_font(VteTerminal *terminal);
void create_bash(GtkWidget *terminal, GtkWidget *notebook);
void update_tab_label(VteTerminal *terminal);

#endif // TERMINAL_H
