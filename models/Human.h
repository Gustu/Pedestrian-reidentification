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
    Human() : lost(false) {};

    HistDescriptor histDescriptor;
    GaborDescriptor gaborDescriptor;
    Kalman kalman;
    OpticalFlow opticalFlow;

    cv::Point point;
    int id;
    cv::Scalar color;

    bool lost;
};


#endif //REIDENTIFICATION_HUMAN_H
