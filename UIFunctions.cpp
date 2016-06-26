//
// Created by hya on 26.06.16.
//

#include "UIFunctions.h"

extern "C" G_MODULE_EXPORT void on_startButton_clicked(GtkButton *button, gpointer user_data) {
    cout << "lol" << endl;
}

extern "C" G_MODULE_EXPORT void on_fileChooserButton_file_set(GtkFileChooser *fileChooser) {
    fileName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
}




