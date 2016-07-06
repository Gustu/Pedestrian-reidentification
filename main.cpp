#include "main.h"


//#define DEMO

GtkWidget *createWindow();
ReidentificationAlg *reidentificationAlg;

int main(int argc, char *argv[]) {
#if defined DEMO
    GtkWidget * window;
    gtk_set_locale();
    gtk_init( & argc, & argv );
    window = createWindow();
    gtk_widget_show( window );
    gtk_main();
#else
    ReidentificationData data;
    reidentificationAlg = new ReidentificationAlg(data);
    reidentificationAlg->setFileName((char *) "video/campus4-c0.avi", data);
    reidentificationAlg->init(data);
    reidentificationAlg->start(data);
#endif
    return 0;
}

GtkWidget *createWindow() {
    GtkWidget *window;
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, UI_FILE, &error)) {
        g_warning("Can not open builder file: %s", error->message);
        g_error_free(error);
    }

    gtk_builder_connect_signals(builder, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

    g_object_unref(builder);

    return window;
}

