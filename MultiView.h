//
// Created by hya on 06.07.16.
//

#ifndef REIDENTIFICATION_MULTIVIEW_H
#define REIDENTIFICATION_MULTIVIEW_H

#include <vector>
#include "View.h"

using namespace std;

class MultiView {
public:
    vector<View> views;

    ReidentificationAlg *alg;

    MultiView() {
        alg = new ReidentificationAlg();
        View view((char *) "video/campus4-c0.avi", 0);
        views.push_back(view);
        View view2((char *) "video/campus4-c1.avi", 1);
        views.push_back(view2);
    }

    void start();
};


#endif //REIDENTIFICATION_MULTIVIEW_H
