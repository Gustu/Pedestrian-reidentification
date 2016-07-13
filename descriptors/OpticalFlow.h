//
// Created by hya on 21.05.16.
//

#ifndef REIDENTIFICATION_OPTICALFLOW_H
#define REIDENTIFICATION_OPTICALFLOW_H

#define LOST_LIMIT 0.5

#define NOT_FOUND_POINTS 0.9

#include <cv.hpp>
#include "OpticalFlowData.h"

using namespace cv;
using namespace std;

class OpticalFlow {
public:
    OpticalFlowData data;

    void resetNotFoundPoints(vector<uchar> status, Rect lastRect);

    OpticalFlow();

    void calculate(Mat prev, Mat img, Rect lastRect, bool clear);

    void getOpticalFlowPoints(const Rect &rect, Mat &img);

    void drawPoints(Mat img, Scalar scalar_);

    void swapPoints();

    void calculateWithCollision(Point move);

    void reset();

    bool isTrackingLost(Mat &img);

private:
    void calculateRegion();

    void movePoints(int i, int i1);
};


#endif //REIDENTIFICATION_OPTICALFLOW_H
