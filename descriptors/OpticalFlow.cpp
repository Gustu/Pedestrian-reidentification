//
// Created by hya on 21.05.16.
//

#include "OpticalFlow.h"

void OpticalFlow::calculate(Mat &prev, Mat &img, Point lastPoint) {
    if (!points[0].empty()) {
        vector<uchar> status;
        vector<float> err;
        calcOpticalFlowPyrLK(prev, img, points[0], points[1], status, err, winSize, 5, termcrit, 0, 0.001);
        resetNotFoundPoints(status, lastPoint);
    }
}

OpticalFlow::OpticalFlow() {
    termcrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 40, 0.03);
    subPixWinSize = Size(10, 10);
    winSize = Size(7, 7);
}

void OpticalFlow::resetNotFoundPoints(vector<uchar> status, Point lastPoint) {
    if(points[0].size() == points[1].size() && points[0].size() == status.size()) {
        for (int i = 0; i < points->size(); i++) {
            if (status[i] == 0) {
                points[0][i] = lastPoint;
                points[1][i] = lastPoint;
            }
        }
    }
}

void OpticalFlow::getOpticalFlowPoints(const Rect &rect, Mat &img) {
    Mat grayTrimmed(img, rect);
    goodFeaturesToTrack(grayTrimmed, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
    cornerSubPix(grayTrimmed, points[1], subPixWinSize, Size(-1, -1), termcrit);
    for (Point2f &p : points[1]) {
        p.x += rect.x;
        p.y += rect.y;
    }
}

void OpticalFlow::drawPoints(Mat img, Scalar color) {
    for (Point2f p : points[1]) {
        circle(img, p, 3, color);
    }
}

void OpticalFlow::swapPoints() {
    std::swap(points[1], points[0]);
}




