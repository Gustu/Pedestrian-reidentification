//
// Created by hya on 15.05.16.
//

#ifndef REIDENTIFICATION_GABORDESCRIPTOR_H
#define REIDENTIFICATION_GABORDESCRIPTOR_H


#include <opencv2/core/mat.hpp>
#include "FilterBank.h"
#include "GaborData.h"

using namespace std;
using namespace cv;

class GaborDescriptor {
public:
    GaborData data;
    GaborDescriptor() {}

    void extractFeatures(Mat img, Rect rect);
    double compare(GaborDescriptor descriptor);
};


#endif //REIDENTIFICATION_GABORDESCRIPTOR_H
