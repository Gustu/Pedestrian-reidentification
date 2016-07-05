//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_HUMANDATA_H
#define REIDENTIFICATION_HUMANDATA_H

#include <opencv2/core/types.hpp>
#include <list>
#include "../descriptors/HistDescriptor.h"
#include "../descriptors/GaborDescriptor.h"
#include "../descriptors/Kalman.h"
#include "../descriptors/OpticalFlow.h"

class HumanData {
public:
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
};


#endif //REIDENTIFICATION_HUMANDATA_H
