//
// Created by hya on 10.04.16.
//

#include "Human.h"

void Human::addToTrack(cv::Point p) {
    if(track.size() < MAX_TRACK) {
        track.push_back(p);
    } else {
        track.pop_front();
        track.push_back(p);
    }
}