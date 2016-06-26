//
// Created by hya on 20.05.16.
//

#ifndef REIDENTIFICATION_KALMAN_H
#define REIDENTIFICATION_KALMAN_H


#include <cv.hpp>
#include <list>
#include <queue>
#include "opencv2/video/tracking.hpp"

#define THRESH_LOST_TRACKING 0.2

using namespace cv;
using namespace std;

class Kalman {
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

    Kalman();

    void predict(double dt);

    void drawPredicted(Mat &img);

    void update(Rect &rect);

    void resetCounter();

    void checkIfLostTracking(Mat &img);
};




#endif //REIDENTIFICATION_KALMAN_H
