#include "../include/app.h"
#include "../include/window.h"

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("io.github.nhrx1337.vara", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
