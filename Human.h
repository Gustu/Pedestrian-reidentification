//
// Created by hya on 10.04.16.
//

#ifndef REIDENTIFICATION_HUMAN_H
#define REIDENTIFICATION_HUMAN_H

#include <opencv2/core/types.hpp>
#include <list>
#include "descriptors/HistDescriptor.h"

class Human {
public:
    Human() {};

    HistDescriptor descriptor;

    cv::Point point;
    int id;
    cv::Scalar color;
};


#endif //REIDENTIFICATION_HUMAN_H
