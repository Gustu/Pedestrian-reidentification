//
// Created by hya on 26.06.16.
//

#ifndef REIDENTIFICATION_UIFUNCTIONS_H
#define REIDENTIFICATION_UIFUNCTIONS_H

#include <gtk/gtk.h>
#include <iostream>

using namespace std;

char *fileName;

extern "C" G_MODULE_EXPORT void on_startButton_clicked(GtkButton *button, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_fileChooserButton_file_set(GtkFileChooser *fileChooser);

#endif //REIDENTIFICATION_UIFUNCTIONS_H
