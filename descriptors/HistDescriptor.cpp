//
// Created by hya on 03.05.16.
//

#include "HistDescriptor.h"

void HistDescriptor::extractFeatures(Mat &currentImage, Rect &rect, Mat &m) {
    Mat img(currentImage, rect);
    Mat mask(m, rect);
    Mat hsv;
    cvtColor(img, hsv, CV_BGR2HSV);

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = {0, 180};
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float sranges[] = {0, 256};
    const float *ranges[] = {hranges, sranges};
    MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};

    calcHist(&hsv, 1, channels, mask, hist, 2, histSize, ranges, true, false);

    this->hist = hist;
}

double HistDescriptor::compare(HistDescriptor &descriptor) {
    return compareHist(this->hist, descriptor.hist, HISTCMP_BHATTACHARYYA);
}

void HistDescriptor::thresholdHist(MatND &mat, double d) {

}





