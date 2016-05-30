//
// Created by hya on 10.04.16.
//

#ifndef REIDENTIFICATION_HUMAN_H
#define REIDENTIFICATION_HUMAN_H

#include <opencv2/core/types.hpp>
#include <list>
#include "../descriptors/HistDescriptor.h"
#include "../descriptors/GaborDescriptor.h"
#include "../descriptors/Kalman.h"
#include "../descriptors/OpticalFlow.h"

class Human {
public:
    Human() : lost(false), outOfWindow(false), collision(-1), hist(0) { };

    HistDescriptor histDescriptor;
    GaborDescriptor gaborDescriptor;
    Kalman kalman;
    OpticalFlow opticalFlow;

    Rect boudingBox;
    Point point;
    Point predicted;
    int id;
    Scalar color;

    bool collision;
    bool reinit;

    Point before;
    Point after;
    Point move;
    int hist;

    bool lost;
    bool outOfWindow;

    void predictHumanPosition();

    void calcBeforeAfter(Point p);
};


#endif //REIDENTIFICATION_HUMAN_H
