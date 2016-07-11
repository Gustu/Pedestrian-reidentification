//
// Created by hya on 06.07.16.
//

#ifndef REIDENTIFICATION_VIEW_H
#define REIDENTIFICATION_VIEW_H

#include "ReidentificationData.h"

class View {
public:
    int id;
    static int sNextId;
    int getNextId();
    String winname = "Press q to exit";

    ReidentificationData data;

    View(char *fileName);
};




#endif //REIDENTIFICATION_VIEW_H
