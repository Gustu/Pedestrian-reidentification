//
// Created by hya on 20.05.16.
//

#include "Kalman.h"



Kalman::Kalman() {
    data.stateSize = 6;
    data.measSize = 4;
    data.contrSize = 0;
    data.type = CV_32F;
    data.found = false;
    data.lostTracking = false;
    data.notFoundCount = 0;
    data.kf = new KalmanFilter(data.stateSize, data.measSize, data.contrSize, data.type);
    data.state = Mat(data.stateSize, 1, data.type);
    data.meas = Mat(data.measSize, 1, data.type);

    // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

    // Transition State Matrix A
    // Note: set dT at each processing step!
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    setIdentity(data.kf->transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    data.kf->measurementMatrix = Mat::zeros(data.measSize, data.stateSize, data.type);
    data.kf->measurementMatrix.at<float>(0) = 1.0f;
    data.kf->measurementMatrix.at<float>(7) = 1.0f;
    data.kf->measurementMatrix.at<float>(16) = 1.0f;
    data.kf->measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Qhttps://scontent-waw1-1.xx.fbcdn.net/v/t1.0-0/p206x206/25826_374708229561_24411_n.jpg?oh=73088d1ade86fadf91d4e4c41937141c&oe=57DC73BD
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    data.kf->processNoiseCov.at<float>(0) = 1e-2;
    data.kf->processNoiseCov.at<float>(7) = 1e-2;
    data.kf->processNoiseCov.at<float>(14) = 5.0f;
    data.kf->processNoiseCov.at<float>(21) = 5.0f;
    data.kf->processNoiseCov.at<float>(28) = 1e-2;
    data.kf->processNoiseCov.at<float>(35) = 1e-2;
}

void Kalman::predict(double dt) {
    if(data.found) {
        // >>>> Matrix A
        data.kf->transitionMatrix.at<float>(2) = dt;
        data.kf->transitionMatrix.at<float>(9) = dt;
        // <<<< Matrix A

        data.state = data.kf->predict();
        data.predRect.width = (int) data.state.at<float>(4);
        data.predRect.height = (int) data.state.at<float>(5);
        data.predRect.x = (int) (data.state.at<float>(0) - data.predRect.width / 2);
        data.predRect.y = (int) (data.state.at<float>(1) - data.predRect.height / 2);

        data.center.x = (int) data.state.at<float>(0);
        data.center.y = (int) data.state.at<float>(1);
    }
}

void Kalman::drawPredicted(Mat &img) {
    circle(img, data.center, 2, CV_RGB(255,0,0), -1);
    rectangle(img, data.predRect, CV_RGB(255,0,0), 2);
}

void Kalman::update(Rect &rect) {
    data.meas.at<float>(0) = rect.x + rect.width / 2;
    data.meas.at<float>(1) = rect.y + rect.height / 2;
    data.meas.at<float>(2) = (float)rect.width;
    data.meas.at<float>(3) = (float)rect.height;

    if (!data.found) // First detection!
    {
        // >>>> Initialization
        data.kf->errorCovPre.at<float>(0) = 1; // px
        data.kf->errorCovPre.at<float>(7) = 1; // px
        data.kf->errorCovPre.at<float>(14) = 1;
        data.kf->errorCovPre.at<float>(21) = 1;
        data.kf->errorCovPre.at<float>(28) = 1; // px
        data.kf->errorCovPre.at<float>(35) = 1; // px

        data.state.at<float>(0) = data.meas.at<float>(0);
        data.state.at<float>(1) = data.meas.at<float>(1);
        data.state.at<float>(2) = 0;
        data.state.at<float>(3) = 0;
        data.state.at<float>(4) = data.meas.at<float>(2);
        data.state.at<float>(5) = data.meas.at<float>(3);
        // <<<< Initialization

        data.found = true;
    }
    else
        data.kf->correct(data.meas); // Kalman Correction
}

void Kalman::resetCounter() {
    data.notFoundCount = 0;
}







