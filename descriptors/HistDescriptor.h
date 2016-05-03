//
// Created by hya on 03.05.16.
//

#ifndef REIDENTIFICATION_HISTDESCRIPTOR_H
#define REIDENTIFICATION_HISTDESCRIPTOR_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class HistDescriptor {
public:
    MatND hist;

    void extractFeatures(Mat &img, Rect &rect, Mat &mask);
    double compare(HistDescriptor &descriptor);

private:
    void thresholdHist(MatND &mat, double d);
};


#endif //REIDENTIFICATION_HISTDESCRIPTOR_H
