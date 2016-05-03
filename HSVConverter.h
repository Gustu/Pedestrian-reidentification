//
// Created by hya on 03.05.16.
//

#ifndef REIDENTIFICATION_HSVCONVERTER_H
#define REIDENTIFICATION_HSVCONVERTER_H

#include <cmath>

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

static hsv rgb2hsv(rgb in);

static rgb hsv2rgb(hsv in);

#endif //REIDENTIFICATION_HSVCONVERTER_H
