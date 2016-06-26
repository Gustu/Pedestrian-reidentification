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
    if (!collision) {
        if (++hist > 2) {
            before = after;
        }
        after = p;
        moveHistory.push_back(Point(after.x - before.x, after.y - before.y));
        if (moveHistory.size() > MOVE_HISTORY) {
            moveHistory.erase(moveHistory.begin());
        }
        int sumX = 0, sumY = 0;
        for(int i = 0 ; i < moveHistory.size(); i++) {
            sumX += moveHistory[i].x;
            sumY += moveHistory[i].y;
        }
        move.x = sumX / MOVE_HISTORY;
        move.y = sumY / MOVE_HISTORY;
    }
}




