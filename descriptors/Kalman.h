//
// Created by hya on 20.05.16.
//

#ifndef REIDENTIFICATION_KALMAN_H
#define REIDENTIFICATION_KALMAN_H


#include <cv.hpp>
#include <list>
#include <queue>
#include "opencv2/video/tracking.hpp"
#include "KalmanData.h"

#define THRESH_LOST_TRACKING 0.2

using namespace cv;
using namespace std;

class Kalman {
public:
    KalmanData data;

    Kalman();

    void predict(double dt);

    void drawPredicted(Mat &img);

    void update(Rect &rect);

    void resetCounter();
};




#endif //REIDENTIFICATION_KALMAN_H
