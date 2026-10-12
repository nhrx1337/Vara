#include "../include/callback.h"
#include "../include/terminal.h"
#include "../include/window.h"

void child_exited(VteTerminal *terminal, gint status, gpointer data) {
    if (!terminal) return;

    GtkWidget *window = 
        gtk_widget_get_ancestor(GTK_WIDGET(terminal), GTK_TYPE_WINDOW);

    if (window) {
        GtkWidget *focused = gtk_window_get_focus(GTK_WINDOW(window));
        if (focused == GTK_WIDGET(terminal) || 
                (focused && gtk_widget_is_ancestor(focused, GTK_WIDGET(terminal)))) {
            gtk_window_set_focus(GTK_WINDOW(window), NULL);
        }
    }

    GtkWidget *notebook_widget = 
        gtk_widget_get_ancestor(GTK_WIDGET(terminal), GTK_TYPE_NOTEBOOK);

    if (!notebook_widget) return;
    GtkNotebook *notebook = GTK_NOTEBOOK(notebook_widget);

    GtkWidget *current = GTK_WIDGET(terminal);

    while (current) {
        gint page_num = gtk_notebook_page_num(notebook, current);
        if (page_num != -1) {
            gtk_notebook_remove_page(notebook, page_num);
            break;
        }

        GtkWidget *parent = gtk_widget_get_parent(current);
        if (!parent) break;

        if (GTK_IS_PANED(parent)) {
            GtkPaned *paned = GTK_PANED(parent);
            if (gtk_paned_get_start_child(paned) == current) {
                gtk_paned_set_start_child(paned, NULL);
            } else if (gtk_paned_get_end_child(paned) == current) {
                gtk_paned_set_end_child(paned, NULL);
            }

            if (gtk_paned_get_start_child(paned) != NULL || 
                    gtk_paned_get_end_child(paned) != NULL) {
                break;
            }

            current = parent;
        } else {
            break;
        }
    }

    if (gtk_notebook_get_n_pages(notebook) == 0) {
        GtkWidget *window = 
            gtk_widget_get_ancestor(GTK_WIDGET(notebook), GTK_TYPE_WINDOW);
        if (window) {
            gtk_window_close(GTK_WINDOW(window));
        }
    }
}

gboolean on_terminal_key_pressed(GtkEventControllerKey *controller, 
        guint keyval, guint keycode, 
        GdkModifierType state, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(data);

    guint expected_modifiers = GDK_CONTROL_MASK | GDK_SHIFT_MASK;
    guint clean_state = state & gtk_accelerator_get_default_mod_mask();

    if (clean_state == expected_modifiers) {
        if (keyval == GDK_KEY_P || keyval == GDK_KEY_p) {
            increase_font(terminal);
            return TRUE;
        }
        else if (keyval == GDK_KEY_O || keyval == GDK_KEY_o) {
            decrease_font(terminal);
            return TRUE;
        }
        else if (keyval == GDK_KEY_I || keyval == GDK_KEY_i) {
            reset_font(terminal);
            return TRUE;
        }
        else if (keyval == GDK_KEY_C || keyval == GDK_KEY_c) {
            vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
            return TRUE;
        }
        else if (keyval == GDK_KEY_V || keyval == GDK_KEY_v) {
            vte_terminal_paste_clipboard(terminal);
            return TRUE;
        }
        else if (keyval == GDK_KEY_M || keyval == GDK_KEY_m) {
            GtkWidget *notebook_widget = 
                gtk_widget_get_ancestor(GTK_WIDGET(terminal), GTK_TYPE_NOTEBOOK);
            if (notebook_widget) {
                GtkNotebook *notebook = GTK_NOTEBOOK(notebook_widget);
                gboolean show_tabs = gtk_notebook_get_show_tabs(notebook);
                gtk_notebook_set_show_tabs(notebook, !show_tabs);
            }
            return TRUE;
        }
    }

    return FALSE;
}

void on_termprop_changed(VteTerminal *terminal, 
        const char *name, gpointer user_data) {
    if (g_strcmp0(name, VTE_TERMPROP_CURRENT_DIRECTORY_URI) != 0)
        return;

    update_tab_label(terminal);
}

void on_terminal_focus_changed(GObject *object, GParamSpec *pspec, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(object);

    if (gtk_widget_has_focus(GTK_WIDGET(terminal)))
        update_tab_label(terminal);
}

static void switch_tab(GtkNotebook *notebook, int direction) {
    int current_page = gtk_notebook_get_current_page(notebook);
    int n_pages = gtk_notebook_get_n_pages(notebook);
    if (n_pages > 1) {
        int new_page = (current_page + direction + n_pages) % n_pages;
        gtk_notebook_set_current_page(notebook, new_page);
    }
}

gboolean on_key_pressed(GtkEventControllerKey *controller,
        guint keyval, guint keycode, 
        GdkModifierType state, gpointer data) {
    AppWidgets *app = (AppWidgets*)data;

    guint expected_modifiers = GDK_CONTROL_MASK | GDK_SHIFT_MASK;
    guint clean_state = state & gtk_accelerator_get_default_mod_mask();

    if (clean_state == expected_modifiers) {
        if (keyval == GDK_KEY_T || keyval == GDK_KEY_t) {
            create_tab(NULL, app->notebook);
            return TRUE;
        }
        else if (keyval == GDK_KEY_E || keyval == GDK_KEY_e) {
            split_terminal(GTK_NOTEBOOK(app->notebook), GTK_ORIENTATION_HORIZONTAL);
            return TRUE;
        }
        else if (keyval == GDK_KEY_W || keyval == GDK_KEY_w) {
            split_terminal(GTK_NOTEBOOK(app->notebook), GTK_ORIENTATION_VERTICAL);
            return TRUE;
        }
        else if (keyval == GDK_KEY_Left) {
            switch_tab(GTK_NOTEBOOK(app->notebook), -1);
            return TRUE;
        }
        else if (keyval == GDK_KEY_Right) {
            switch_tab(GTK_NOTEBOOK(app->notebook), 1);
            return TRUE;
        }
        else {
            int page_num = -1;
            switch (keyval) {
                case GDK_KEY_1: case GDK_KEY_exclam:     page_num = 0; break;
                case GDK_KEY_2: case GDK_KEY_at:         page_num = 1; break;
                case GDK_KEY_3: case GDK_KEY_numbersign: page_num = 2; break;
                case GDK_KEY_4: case GDK_KEY_dollar:     page_num = 3; break;
                case GDK_KEY_5: case GDK_KEY_percent:    page_num = 4; break;
                case GDK_KEY_6: case GDK_KEY_asciicircum:page_num = 5; break;
                case GDK_KEY_7: case GDK_KEY_ampersand:  page_num = 6; break;
                case GDK_KEY_8: case GDK_KEY_asterisk:   page_num = 7; break;
                case GDK_KEY_9: case GDK_KEY_parenleft:  page_num = 8; break;
                default: break;
            }

            if (page_num != -1) {
                if (page_num < gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook))) {
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), page_num);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
