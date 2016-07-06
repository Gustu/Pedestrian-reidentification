//
// Created by hya on 26.06.16.
//

#ifndef REIDENTIFICATION_UIFUNCTIONS_H
#define REIDENTIFICATION_UIFUNCTIONS_H

#include <gtk/gtk.h>
#include <iostream>
#include "ReidentificationAlg.h"


using namespace std;

extern char *fileNameUI;
extern ReidentificationData data;
extern ReidentificationAlg *r;
extern bool started;

extern "C" G_MODULE_EXPORT void on_startButton_clicked(GtkButton *button, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_fileChooserButton_file_set(GtkFileChooser *fileChooser);
extern "C" G_MODULE_EXPORT void on_stopButton_clicked(GtkButton *button, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_window1_delete_event();

void createThread();

#endif //REIDENTIFICATION_UIFUNCTIONS_H
