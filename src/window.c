#include "../include/window.h"
#include "../include/terminal.h"
#include "../include/callback.h"

void add_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, 
            "/io/github/nhrx1337/vara/style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(provider);
}

void create_tab(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data); 

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *terminal = vte_terminal_new();
    GtkWidget *tab_label = gtk_label_new("bash");

    create_bash(terminal, GTK_WIDGET(paned));

    gtk_paned_set_start_child(GTK_PANED(paned), terminal);
    gtk_notebook_append_page(notebook, paned, tab_label);
}

void split_terminal(GtkNotebook *notebook, GtkOrientation orientation) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(notebook)));
    GtkWidget *focused_widget = gtk_window_get_focus(GTK_WINDOW(window));

    if (!focused_widget || !VTE_IS_TERMINAL(focused_widget)) {
        gint current_page = gtk_notebook_get_current_page(notebook);
        if (current_page == -1) return;
        focused_widget = gtk_notebook_get_nth_page(notebook, current_page);
        if (!GTK_IS_PANED(focused_widget)) return;
    }

    GtkWidget *parent = gtk_widget_get_parent(focused_widget);
    
    if (GTK_IS_PANED(parent)) {
        GtkPaned *paned = GTK_PANED(parent);
        
        g_object_ref(focused_widget);
        
        gboolean is_start = 
            (gtk_paned_get_start_child(paned) == focused_widget);
        if (is_start) {
            gtk_paned_set_start_child(paned, NULL);
        } else {
            gtk_paned_set_end_child(paned, NULL);
        }

        GtkWidget *sub_paned = gtk_paned_new(orientation);
        
        GtkWidget *new_terminal = vte_terminal_new();
        create_bash(new_terminal, GTK_WIDGET(notebook));

        gtk_paned_set_start_child(GTK_PANED(sub_paned), focused_widget);
        gtk_paned_set_end_child(GTK_PANED(sub_paned), new_terminal);
        g_object_unref(focused_widget);

        if (is_start) {
            gtk_paned_set_start_child(paned, sub_paned);
        } else {
            gtk_paned_set_end_child(paned, sub_paned);
        }

        gtk_widget_grab_focus(new_terminal);
    }
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder;
    GtkWidget *window, *notebook;

    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-interface-color-scheme", 
            GTK_INTERFACE_COLOR_SCHEME_DARK, NULL);

    builder = gtk_builder_new_from_resource("/io/github/nhrx1337/vara/vara.ui");
    add_css();
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    notebook = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));

    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);

    gtk_window_set_application(GTK_WINDOW(window), app);

    create_tab(NULL, notebook);

    AppWidgets *widgets = g_new(AppWidgets, 1);
    widgets->notebook = notebook;

    GtkEventController *key_controller = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(
            key_controller, GTK_PHASE_CAPTURE);
    g_signal_connect(key_controller, "key-pressed", 
            G_CALLBACK(on_key_pressed), widgets);
    gtk_widget_add_controller(window, key_controller);

    g_signal_connect_swapped(window, "destroy", 
            G_CALLBACK(g_free), widgets);
    g_object_unref(builder);

    gtk_window_present(GTK_WINDOW(window));
}
