//
// Created by hya on 15.05.16.
//

#ifndef REIDENTIFICATION_GABORDESCRIPTOR_H
#define REIDENTIFICATION_GABORDESCRIPTOR_H


#include <opencv2/core/mat.hpp>
#include "FilterBank.h"

using namespace std;
using namespace cv;

class GaborDescriptor {
public:
    FilterBank filterBank;
    GaborDescriptor() {}
    vector<double> means;
    vector<double> stdevs;
    void extractFeatures(Mat img, Rect rect);
    double compare(GaborDescriptor descriptor);
};


#endif //REIDENTIFICATION_GABORDESCRIPTOR_H
