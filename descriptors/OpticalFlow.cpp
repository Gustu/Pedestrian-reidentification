//
// Created by hya on 21.05.16.
//

#include <iostream>
#include "OpticalFlow.h"

void OpticalFlow::calculate(Mat prev, Mat img, Rect lastRect, bool clear) {
    if (clear) {
        data.boundingBox.width = 0;
        data.boundingBox.height = 0;
        data.points->clear();
    }
    if (!data.points[0].empty() && !data.blocked) {
        vector<uchar> status;
        vector<float> err;
        calcOpticalFlowPyrLK(prev, img, data.points[0], data.points[1], status, err, data.winSize, 5, data.termcrit, 0, 0.001);
        resetNotFoundPoints(status, lastRect);
        movePoints(lastRect.width, lastRect.height);
        calculateRegion();
    }
}

OpticalFlow::OpticalFlow() {
    data.termcrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 40, 0.03);
    data.subPixWinSize = Size(10, 10);
    data.winSize = Size(7, 7);
    data.blocked = false;
}

void OpticalFlow::resetNotFoundPoints(vector<uchar> status, Rect lastRect) {
    if (data.points[0].size() == data.points[1].size() && data.points[0].size() == status.size()) {
        int count = 0;
        for (int i = 0; i < data.points->size(); i++) {
            if (status[i] == 0) {
                count++;
            }
        }
        if((double)count/status.size() > NOT_FOUND_POINTS) return;
        for (int i = 0; i < data.points->size(); i++) {
            if (status[i] == 0) {
                Point randed(lastRect.x + rand() % lastRect.width, lastRect.y + rand() % lastRect.height);
                data.points[0][i] = randed;
                data.points[1][i] = randed;
            }
        }
    }
}

void OpticalFlow::getOpticalFlowPoints(const Rect &rect, Mat &gray) {
    if (rect.x >= 0 && rect.y >= 0 && rect.x + rect.width < gray.cols && rect.y + rect.height < gray.rows) {
        Mat grayTrimmed(gray, rect);
        goodFeaturesToTrack(grayTrimmed, data.points[1], data.MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
        if (!data.points[1].empty()) {
            if (rect.width > data.subPixWinSize.width * 2 + 5 && rect.height > data.subPixWinSize.height * 2 + 5)
                cornerSubPix(grayTrimmed, data.points[1], data.subPixWinSize, Size(-1, -1), data.termcrit);
            for (Point2f &p : data.points[1]) {
                p.x += rect.x; // project points to image
                p.y += rect.y; // project points to image
            }
        }
    }
}

void OpticalFlow::drawPoints(Mat img, Scalar color) {
    for (Point2f p : data.points[1]) {
        circle(img, p, 3, color);
    }
    circle(img, data.centroid, 3, color, -1);
    rectangle(img, data.boundingBox, color);
}

void OpticalFlow::swapPoints() {
    if (!data.blocked) std::swap(data.points[1], data.points[0]);
}

void OpticalFlow::calculateRegion() {
    int countx = std::accumulate(data.points[1].begin(), data.points[1].end(), 0,
                                 [](int current_sum, Point2f const &value) { return current_sum + value.x; });
    int county = std::accumulate(data.points[1].begin(), data.points[1].end(), 0,
                                 [](int current_sum, Point2f const &value) { return current_sum + value.y; });
    data.centroid.x = (int) (countx / data.points[1].size());
    data.centroid.y = (int) (county / data.points[1].size());
    Rect r = boundingRect(data.points[1]);
    data.boundingBox = r;
    if (abs(data.centroid.x - data.boundingBox.x) > abs(data.centroid.x - data.boundingBox.x + data.boundingBox.width)) {
        data.boundingBox.x = data.centroid.x - abs(data.centroid.x - data.boundingBox.x + data.boundingBox.width);
        data.boundingBox.width = 2 * abs(data.centroid.x - data.boundingBox.x + data.boundingBox.width);
    } else {
        data.boundingBox.x = data.centroid.x - abs(data.centroid.x - data.boundingBox.x);
        data.boundingBox.width = 2 * abs(data.centroid.x - data.boundingBox.x);
    }
}

void OpticalFlow::movePoints(int lastWidth, int lastHeight) {
    if (data.centroid.x != 0 && data.centroid.y != 0) {
        int maxXDistance = lastWidth / 2;
        for (Point2f &p : data.points[1]) {
            int xDistance = data.centroid.x - p.x;
            if (abs(xDistance) > maxXDistance) {
                if (xDistance > 0) {
                    p.x -= maxXDistance - abs(xDistance);
                } else {
                    p.x += maxXDistance - abs(xDistance);
                }
            }
        }
    }
}

void OpticalFlow::calculateWithCollision(Point move) {

    for (int i = 0; i < data.points[0].size(); i++) {
        data.points[0][i].x += move.x;
        data.points[0][i].y += move.y;
    }

    for (int i = 0; i < data.points[1].size(); i++) {
        data.points[1][i].x += move.x;
        data.points[1][i].y += move.y;
    }

    data.boundingBox = boundingRect(data.points[0]);
}

void OpticalFlow::reset() {
    data.points->clear();
}

bool OpticalFlow::checkIfLostTracking(Mat &img) {
    int count = 0;
    for (Point p : data.points[1]) {
        if (p.x > img.cols || p.x < 0 || p.y > img.rows || p.y < 0) {
            count++;
            continue;
        }
        Vec3f c = img.at<Vec3f>(p);
        if (c(0) == 0 && c(1) == 0 && c(2) == 0) {
            count++;
        }
    }
    return (double) count / data.points[1].size() > LOST_LIMIT;
}











