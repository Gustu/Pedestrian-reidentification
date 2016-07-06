//
// Created by hya on 06.07.16.
//

#include "MultiView.h"

void MultiView::start() {
    while (!views.empty()) {
        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            if (!it->data.cap->isOpened()) {
                alg->finish(it->data, it->winname);
                views.erase(it);
            }
        }

        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            if (it->data.exiting) {
                alg->finish(it->data, it->winname);
                views.erase(it);
            }
            it->data.cap->read(it->data.img);
            if (it->data.img.empty()) {
                alg->finish(it->data, it->winname);
                views.erase(it);
            }
        }

        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            alg->processImage(it->data);
        }

        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            alg->applyAlgorithm(it->data.frame_idx, it->data);
        }

        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            alg->drawing(it->data, it->winname);
        }

        for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
            alg->swap(it->data);

            if (it->data.frame_idx % it->data.detect_interval == 0) {
                alg->markAllAsLost(it->data);
            }

            it->data.frame_idx++;
        }

        if (alg->exit()) break;
    }
}