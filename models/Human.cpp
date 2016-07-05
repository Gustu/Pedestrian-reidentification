//
// Created by hya on 10.04.16.
//

#include "Human.h"




void Human::predictHumanPosition() {
    int kx = data.kalman.data.center.x;
    int ky = data.kalman.data.center.y;
    data.predicted.x = kx;
    data.predicted.y = ky;
}

void Human::calcBeforeAfter(Point p) {
    if (!data.collision) {
        if (++data.hist > 2) {
            data.before = data.after;
        }
        data.after = p;
        data.moveHistory.push_back(Point(data.after.x - data.before.x, data.after.y - data.before.y));
        if (data.moveHistory.size() > MOVE_HISTORY) {
            data.moveHistory.erase(data.moveHistory.begin());
        }
        int sumX = 0, sumY = 0;
        for(int i = 0 ; i < data.moveHistory.size(); i++) {
            sumX += data.moveHistory[i].x;
            sumY += data.moveHistory[i].y;
        }
        data.move.x = sumX / MOVE_HISTORY;
        data.move.y = sumY / MOVE_HISTORY;
    }
}




