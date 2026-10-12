#include "../include/terminal.h"
#include "../include/callback.h"

void increase_font(VteTerminal *terminal) {
    vte_terminal_set_font_scale(VTE_TERMINAL(terminal), 
            vte_terminal_get_font_scale(VTE_TERMINAL(terminal)) + SCALE_FACTOR);
}

void decrease_font(VteTerminal *terminal) {
    vte_terminal_set_font_scale(VTE_TERMINAL(terminal), 
            vte_terminal_get_font_scale(VTE_TERMINAL(terminal)) - SCALE_FACTOR);
}

void reset_font(VteTerminal *terminal) {
    vte_terminal_set_font_scale(VTE_TERMINAL(terminal), 1.0);
}

void set_terminal_colors(VteTerminal *terminal) {
    GdkRGBA foreground, background;
    
    gdk_rgba_parse(&foreground, "#00ff66");
    gdk_rgba_parse(&background, "#0d1117");

    GdkRGBA palette[16];
    const char *color_codes[16] = {
        "#212121", "#F07178", "#C3E88D", "#FFCB6B", 
        "#82AAFF", "#C792EA", "#89DDFF", "#EEFFFF", 
        "#4A4A4A", "#F07178", "#C3E88D", "#FFCB6B", 
        "#82AAFF", "#C792EA", "#89DDFF", "#FFFFFF"  
    };
    for (int i = 0; i < 16; i++) {
        gdk_rgba_parse(&palette[i], color_codes[i]);
    }

    vte_terminal_set_colors(terminal, &foreground, &background, palette, 16);
}

// Uncomment the line below to set a custom font (requires the font to be installed on your system)
/*
void set_terminal_font(VteTerminal *terminal) {
    PangoFontDescription *font_desc = 
        pango_font_description_from_string("MesloLGS Nerd Font 12");
    vte_terminal_set_font(terminal, font_desc);
    pango_font_description_free(font_desc);
}
*/

void update_tab_label(VteTerminal *terminal) {
    GtkWidget *notebook_widget =
        gtk_widget_get_ancestor(GTK_WIDGET(terminal), GTK_TYPE_NOTEBOOK);
    if (!notebook_widget)
        return;

    GtkNotebook *notebook = GTK_NOTEBOOK(notebook_widget);
    GtkWidget *page = gtk_widget_get_parent(GTK_WIDGET(terminal));

    while (page && gtk_notebook_page_num(notebook, page) == -1)
        page = gtk_widget_get_parent(page);

    if (!page) return;

    GUri *uri = vte_terminal_ref_termprop_uri(
        terminal, VTE_TERMPROP_CURRENT_DIRECTORY_URI);

    const char *path = uri ? g_uri_get_path(uri) : NULL;
    const char *host = uri ? g_uri_get_host(uri) : NULL;
    const gchar *user = g_get_user_name();

    gchar *display_path = NULL;
    if (path) {
        const gchar *home = g_get_home_dir();
        if (home && g_str_has_prefix(path, home)) {
            display_path = g_strdup_printf("~%s", path + strlen(home));
        } else {
            display_path = g_strdup(path);
        }
    } else {
        display_path = g_strdup("/");
    }

    gchar *name = g_strdup_printf("%s@%s:%s",
        user ? user : "user",
        (host && *host) ? host : "localhost",
        display_path);

    GtkWidget *label = gtk_notebook_get_tab_label(notebook, page);
    if (GTK_IS_LABEL(label))
        gtk_label_set_text(GTK_LABEL(label), name);

    g_free(name);
    g_free(display_path);
    if (uri) g_uri_unref(uri);
}

void create_bash(GtkWidget *terminal, GtkWidget *notebook) {
    gchar **envp = g_get_environ();
    envp = g_environ_setenv(
        envp,
        "PROMPT_COMMAND",
        "printf '\\e]7;file://%s%s\\a' \"${HOSTNAME:-localhost}\" \"$PWD\"",
        TRUE
    );

    const gchar *shell = g_environ_getenv(envp, "SHELL");
    if (!shell) {
        shell = "/bin/sh";
    }

    gchar *command[2];
    command[0] = g_strdup(shell);
    command[1] = NULL;

    vte_terminal_set_scroll_on_output(VTE_TERMINAL(terminal), TRUE);
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(terminal), TRUE);
    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), TRUE);

    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,     
        command,    
        envp,
        (GSpawnFlags)0,         
        NULL, NULL,  
        NULL,     
        -1,       
        NULL,     
        NULL,
        NULL);

    g_free(command[0]);
    g_strfreev(envp);

    set_terminal_colors(VTE_TERMINAL(terminal));
    //set_terminal_font(VTE_TERMINAL(terminal));

    GtkEventController *term_key_controller = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(term_key_controller, GTK_PHASE_CAPTURE);
    g_signal_connect(term_key_controller, "key-pressed", G_CALLBACK(on_terminal_key_pressed), terminal);
    gtk_widget_add_controller(terminal, term_key_controller);

    g_signal_connect(terminal, "child-exited", G_CALLBACK(child_exited), notebook);
    g_signal_connect(terminal, "termprop-changed", G_CALLBACK(on_termprop_changed), NULL);
    g_signal_connect(terminal, "notify::has-focus", G_CALLBACK(on_terminal_focus_changed), NULL);
}
