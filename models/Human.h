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

#define MOVE_HISTORY 3

class Human {
public:
    Human() : lostTracking(false), outOfWindow(false), collision(-1), hist(0) { };

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
    vector<Point> moveHistory;
    Point move;
    int hist;

    bool lostTracking;
    bool outOfWindow;

    void predictHumanPosition();

    void calcBeforeAfter(Point p);
};


#endif //REIDENTIFICATION_HUMAN_H
