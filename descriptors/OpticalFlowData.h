//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_OPTICALFLOWDATA_H
#define REIDENTIFICATION_OPTICALFLOWDATA_H

#include <cv.hpp>

using namespace cv;
using namespace std;

class OpticalFlowData {
public:
    vector<Point2f> points[2];
    TermCriteria termcrit;
    Size subPixWinSize;
    Size winSize;
    const int MAX_COUNT = 500;

    Point centroid;
    bool blocked;
    Rect boundingBox;
};


#endif //REIDENTIFICATION_OPTICALFLOWDATA_H
