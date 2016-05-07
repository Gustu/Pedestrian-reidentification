//
// Created by hya on 03.05.16.
//

#include "HistDescriptor.h"

void HistDescriptor::extractFeatures(Mat &currentImage, Rect &rect, Mat &m) {
    Rect up(rect.x, rect.y, rect.width, rect.height/2);
    Rect low(rect.x, rect.y + rect.height/2, rect.width, rect.height/2);
    getHistogram(currentImage, rect, m, full);
    getHistogram(currentImage, up, m, upper);
    getHistogram(currentImage, low, m, lower);
}

void HistDescriptor::getHistogram(const Mat &currentImage, const Rect &rect, const Mat &m, MatND &histogram) {
    Mat img(currentImage, rect);
    Mat mask(m, rect);
    Mat hsv;
    cvtColor(img, hsv, CV_BGR2HSV);

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 35, sbins = 32;
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
    equalizeIntensity(hist);
    histogram = hist;
}

double HistDescriptor::compare(HistDescriptor &descriptor) {
    double full_compare = compareHist(full, descriptor.full, HISTCMP_BHATTACHARYYA);
    if(full_compare > COMPARISON_THRESHOLD) {
        double lower_compare = compareHist(lower, descriptor.lower, HISTCMP_BHATTACHARYYA);
        double upper_compare = compareHist(upper, descriptor.upper, HISTCMP_BHATTACHARYYA);

        return (lower_compare + upper_compare)/2;
    }

    return full_compare;
}

Mat HistDescriptor::equalizeIntensity(const Mat &inputImage) {
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;

        cvtColor(inputImage, ycrcb, CV_HSV2BGR);
        cvtColor(ycrcb,ycrcb,CV_BGR2YCrCb);

        vector<Mat> channels;
        split(ycrcb,channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels,ycrcb);

        cvtColor(ycrcb,result,CV_YCrCb2BGR);

        return result;
    }
    return Mat();
}




