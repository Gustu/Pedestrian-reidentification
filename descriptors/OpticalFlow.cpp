//
// Created by hya on 21.05.16.
//

#include "OpticalFlow.h"

void OpticalFlow::calculate(Mat &prev, Mat &img, Point lastPoint, int lastWidth, int lastHeight) {
    if (!points[0].empty() && !blocked) {
        vector<uchar> status;
        vector<float> err;
        calcOpticalFlowPyrLK(prev, img, points[0], points[1], status, err, winSize, 5, termcrit, 0, 0.001);
        resetNotFoundPoints(status, lastPoint);
        movePoints(lastWidth, lastHeight);
        calculateRegion();
    }
}

OpticalFlow::OpticalFlow() {
    termcrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 40, 0.03);
    subPixWinSize = Size(10, 10);
    winSize = Size(7, 7);
    blocked = false;
}

void OpticalFlow::resetNotFoundPoints(vector<uchar> status, Point lastPoint) {
    if (points[0].size() == points[1].size() && points[0].size() == status.size()) {
        for (int i = 0; i < points->size(); i++) {
            if (status[i] == 0) {
                points[0][i] = lastPoint;
                points[1][i] = lastPoint;
            }
        }
    }
}

void OpticalFlow::getOpticalFlowPoints(const Rect &rect, Mat &gray) {
    Mat grayTrimmed(gray, rect);
    goodFeaturesToTrack(grayTrimmed, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
    if(rect.width > subPixWinSize.width*2 + 5 && rect.height > subPixWinSize.height*2 + 5)cornerSubPix(grayTrimmed, points[1], subPixWinSize, Size(-1, -1), termcrit);
    for (Point2f &p : points[1]) {
        p.x += rect.x; // project points to image
        p.y += rect.y; // project points to image
    }
}

void OpticalFlow::drawPoints(Mat img, Scalar color) {
    for (Point2f p : points[1]) {
        circle(img, p, 3, color);
    }
    circle(img, centroid, 3, color, -1);
    rectangle(img, boundingBox, color);
}

void OpticalFlow::swapPoints() {
    if (!blocked) std::swap(points[1], points[0]);
}

void OpticalFlow::calculateRegion() {
    int countx = std::accumulate(points[1].begin(), points[1].end(), 0,
                                 [](int current_sum, Point2f const &value) { return current_sum + value.x; });
    int county = std::accumulate(points[1].begin(), points[1].end(), 0,
                                 [](int current_sum, Point2f const &value) { return current_sum + value.y; });
    centroid.x = (int) (countx / points[1].size());
    centroid.y = (int) (county / points[1].size());
    Rect r = boundingRect(points[1]);
    boundingBox = r;
    if (abs(centroid.x - boundingBox.x) > abs(centroid.x - boundingBox.x + boundingBox.width)) {
        boundingBox.x = centroid.x - abs(centroid.x - boundingBox.x + boundingBox.width);
        boundingBox.width = 2 * abs(centroid.x - boundingBox.x + boundingBox.width);
    } else {
        boundingBox.x = centroid.x - abs(centroid.x - boundingBox.x);
        boundingBox.width = 2 * abs(centroid.x - boundingBox.x);
    }
}

void OpticalFlow::movePoints(int lastWidth, int lastHeight) {
    if (centroid.x != 0 && centroid.y != 0) {
        int maxXDistance = lastWidth / 2;
//        int maxYDistance = lastHeight / 2;
        for (Point2f &p : points[1]) {
            int xDistance = centroid.x - p.x;
//            int yDistance = centroid.y - p.y;
            if (abs(xDistance) > maxXDistance) {
                if (xDistance > 0) {
                    p.x -= maxXDistance - abs(xDistance);
                } else {
                    p.x += maxXDistance - abs(xDistance);
                }
            }
//            if (abs(yDistance) > maxYDistance) {
//                if (yDistance > 0) {
//                    p.y -= maxYDistance - abs(yDistance);
//                } else {
//                    p.y += maxYDistance - abs(yDistance);
//                }
//            }
        }
    }
}

void OpticalFlow::calculateWithCollision(Point move) {

    for(int i = 0; i< points[0].size(); i++) {
        points[0][i].x += move.x;
        points[0][i].y += move.y;
    }

    for(int i = 0; i< points[1].size(); i++) {
        points[1][i].x += move.x;
        points[1][i].y += move.y;
    }

    boundingBox = boundingRect(points[0]);
}











