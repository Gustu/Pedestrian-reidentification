//
// Created by hya on 06.07.16.
//

#include "View.h"

View::View(char *fileName, int id) {
    winname += to_string(id);
    namedWindow(winname, WINDOW_KEEPRATIO);
    data.fileName = fileName;
    data.exiting = false;
    data.hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
#ifndef RETRAIN_HOG
    data.hogTrainer = new HOGTrainer(Size(64, 128));
#else
    data.hogTrainer = new HOGTrainer("hog/pos/", "hog/pos.list", "hog/neg/", "hog/neg.list", Size(64, 128));
    data.hogTrainer->train();
#endif
    data.angleHog = data.hogTrainer->getHOG();
    data.cap = new VideoCapture(data.fileName);
    data.pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
}