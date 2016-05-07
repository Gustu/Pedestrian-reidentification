//
// Created by hya on 03.05.16.
//

#ifndef REIDENTIFICATION_HISTDESCRIPTOR_H
#define REIDENTIFICATION_HISTDESCRIPTOR_H

#define COMPARISON_THRESHOLD 0.5

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>

using namespace cv;
using namespace std;

class HistDescriptor {
public:
    MatND full;
    MatND upper;
    MatND lower;

    void extractFeatures(Mat &img, Rect &rect, Mat &mask);
    double compare(HistDescriptor &descriptor);

private:
    void getHistogram(const Mat &currentImage, const Rect &rect, const Mat &m, MatND &histogram);
    Mat equalizeIntensity(const Mat& inputImage);
};


#endif //REIDENTIFICATION_HISTDESCRIPTOR_H
