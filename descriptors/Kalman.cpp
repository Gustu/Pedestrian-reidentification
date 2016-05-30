//
// Created by hya on 20.05.16.
//

#include "Kalman.h"

Kalman::Kalman() {
    stateSize = 6;
    measSize = 4;
    contrSize = 0;
    type = CV_32F;
    found = false;
    notFoundCount = 0;
    kf = new KalmanFilter(stateSize, measSize, contrSize, type);
    state = Mat(stateSize, 1, type);
    meas = Mat(measSize, 1, type);

    // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

    // Transition State Matrix A
    // Note: set dT at each processing step!
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    setIdentity(kf->transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    kf->measurementMatrix = Mat::zeros(measSize, stateSize, type);
    kf->measurementMatrix.at<float>(0) = 1.0f;
    kf->measurementMatrix.at<float>(7) = 1.0f;
    kf->measurementMatrix.at<float>(16) = 1.0f;
    kf->measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Qhttps://scontent-waw1-1.xx.fbcdn.net/v/t1.0-0/p206x206/25826_374708229561_24411_n.jpg?oh=73088d1ade86fadf91d4e4c41937141c&oe=57DC73BD
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    kf->processNoiseCov.at<float>(0) = 1e-2;
    kf->processNoiseCov.at<float>(7) = 1e-2;
    kf->processNoiseCov.at<float>(14) = 5.0f;
    kf->processNoiseCov.at<float>(21) = 5.0f;
    kf->processNoiseCov.at<float>(28) = 1e-2;
    kf->processNoiseCov.at<float>(35) = 1e-2;
}

void Kalman::predict(double dt) {
    if(found) {
        // >>>> Matrix A
        kf->transitionMatrix.at<float>(2) = dt;
        kf->transitionMatrix.at<float>(9) = dt;
        // <<<< Matrix A

        state = kf->predict();
        predRect.width = (int) state.at<float>(4);
        predRect.height = (int) state.at<float>(5);
        predRect.x = (int) (state.at<float>(0) - predRect.width / 2);
        predRect.y = (int) (state.at<float>(1) - predRect.height / 2);

        center.x = (int) state.at<float>(0);
        center.y = (int) state.at<float>(1);
    }
}

void Kalman::drawPredicted(Mat &img) {
    circle(img, center, 2, CV_RGB(255,0,0), -1);
    rectangle(img, predRect, CV_RGB(255,0,0), 2);
}

void Kalman::update(Rect &rect) {
    meas.at<float>(0) = rect.x + rect.width / 2;
    meas.at<float>(1) = rect.y + rect.height / 2;
    meas.at<float>(2) = (float)rect.width;
    meas.at<float>(3) = (float)rect.height;

    if (!found) // First detection!
    {
        // >>>> Initialization
        kf->errorCovPre.at<float>(0) = 1; // px
        kf->errorCovPre.at<float>(7) = 1; // px
        kf->errorCovPre.at<float>(14) = 1;
        kf->errorCovPre.at<float>(21) = 1;
        kf->errorCovPre.at<float>(28) = 1; // px
        kf->errorCovPre.at<float>(35) = 1; // px

        state.at<float>(0) = meas.at<float>(0);
        state.at<float>(1) = meas.at<float>(1);
        state.at<float>(2) = 0;
        state.at<float>(3) = 0;
        state.at<float>(4) = meas.at<float>(2);
        state.at<float>(5) = meas.at<float>(3);
        // <<<< Initialization

        found = true;
    }
    else
        kf->correct(meas); // Kalman Correction
}

void Kalman::resetCounter() {
    notFoundCount = 0;
}







