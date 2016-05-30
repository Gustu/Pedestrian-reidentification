//
// Created by hya on 10.04.16.
//

#include "Human.h"


void Human::predictHumanPosition() {
    int kx = kalman.center.x;
    int ky = kalman.center.y;
    predicted.x = kx;
    predicted.y = ky;
}

void Human::calcBeforeAfter(Point p) {
    if(!collision) {
        if(++hist > 2) {
            before = after;
        }
        after = p;
        move.x = after.x - before.x;
        move.y = after.y - before.y;
    }
}




