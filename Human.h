//
// Created by hya on 10.04.16.
//

#ifndef REIDENTIFICATION_HUMAN_H
#define REIDENTIFICATION_HUMAN_H


#include <opencv2/core/types.hpp>
#include <list>
#include "descriptors/KMeansDescriptor.h"

#define MAX_TRACK 10

class Human {
public:
    Human();

    cv::Point point;
    int id;
    cv::Scalar color;
    std::list<cv::Point> track;
    int locked;

    KMeansDescriptor descriptor;

    Human(int id, Scalar c): id(id), color(c) { }

    Human(int id, const cv::Point &point, cv::Scalar color) : id(id), point(point), color(color) {
        track.push_back(point);
        locked = 4;
    }

    void addToTrack(cv::Point p);
};


#endif //REIDENTIFICATION_HUMAN_H
