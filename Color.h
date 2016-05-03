//
// Created by hya on 03.05.16.
//

#ifndef REIDENTIFICATION_COLOR_H
#define REIDENTIFICATION_COLOR_H

#include <opencv2/core/mat.hpp>

using namespace cv;

class Color {
public:
    float probability;
    Vec3f color;

    float compare(Color *color);
    Color(float p, Vec3f c): probability(p), color(c) {}
};


#endif //REIDENTIFICATION_COLOR_H
