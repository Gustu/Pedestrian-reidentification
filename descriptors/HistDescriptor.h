//
// Created by hya on 03.05.16.
//

#ifndef REIDENTIFICATION_HISTDESCRIPTOR_H
#define REIDENTIFICATION_HISTDESCRIPTOR_H

#define COMPARISON_THRESHOLD 0.5
#define HISTORY 3

static const int HOG_HEIGHT_DIVISION = 2;

static const int HOG_WIDTH_DIVISION = 1;

#include <list>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "HistData.h"

using namespace cv;
using namespace std;

class HistDescriptor {
public:
    HistData data;

    HistDescriptor() { data.counter = 0; };

    void extractFeatures(Mat &img, Rect &rect, Mat &mask);

    double compare(HistDescriptor &descriptor);

private:
    MatND getHistogram(const Mat &currentImage, const Rect &rect, const Mat &m);

    Mat equalizeIntensity(const Mat &inputImage);
};


#endif //REIDENTIFICATION_HISTDESCRIPTOR_H
