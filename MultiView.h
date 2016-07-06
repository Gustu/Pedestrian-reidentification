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
        View view((char *) "video/campus4-c0.avi");
        views.push_back(view);
    }

    void start() {
        for(View &view : views) {
            alg->start(view.data, view.winname);
        }
    }
};


#endif //REIDENTIFICATION_MULTIVIEW_H
