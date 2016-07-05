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
#include "HumanData.h"

#define MOVE_HISTORY 3

class Human {
public:
    HumanData data;

    Human() {
        data.lostTracking = false;
        data.outOfWindow = false;
        data.collision = -1;
        data.hist = 0;
    };

    void predictHumanPosition();

    void calcBeforeAfter(Point p);
};


#endif //REIDENTIFICATION_HUMAN_H
