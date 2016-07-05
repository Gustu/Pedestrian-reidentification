//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_KALMANDATA_H
#define REIDENTIFICATION_KALMANDATA_H

#include <list>
#include <queue>
#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace std;

class KalmanData {
public:
    int stateSize;
    int measSize;
    int contrSize;
    int notFoundCount;
    unsigned int type;
    Ptr<KalmanFilter> kf;
    cv::Mat state;  // [x,y,v_x,v_y,w,h]
    cv::Mat meas;    // [z_x,z_y,z_w,z_h]
    bool found;
    Rect predRect;
    Point center;
    bool lostTracking;
};


#endif //REIDENTIFICATION_KALMANDATA_H
