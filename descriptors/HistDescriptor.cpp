//
// Created by hya on 03.05.16.
//

#include <iostream>
#include "HistDescriptor.h"

void HistDescriptor::extractFeatures(Mat &currentImage, Rect &rect, Mat &m) {
    full = getHistogram(currentImage, rect, m);

    int x = rect.x;
    int y = rect.y;
    int dividedHeight = rect.height / HOG_HEIGHT_DIVISION;
    int dividedWidth = rect.width / HOG_WIDTH_DIVISION;
    Rect window;
    window.width = dividedWidth;
    window.height = dividedHeight;
    for (int i = 0; i < HOG_HEIGHT_DIVISION; i++) {
        for (int j = 0; j < HOG_WIDTH_DIVISION; j++) {
            window.x = x + j * dividedWidth;
            window.y = y + i * dividedHeight;
            partials.push_back(getHistogram(currentImage, window, m));
        }
    }
}

MatND HistDescriptor::getHistogram(const Mat &currentImage, const Rect &rect, const Mat &m) {
    Mat img(currentImage, rect);
    Mat mask(m, rect);
    Mat hsv;
    cvtColor(img, hsv, CV_BGR2HSV);

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 20, sbins = 30;
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
    return hist;
}

double HistDescriptor::compare(HistDescriptor &descriptor) {
    double full_compare = 1 - compareHist(full, descriptor.full, HISTCMP_BHATTACHARYYA);

    if (full_compare > COMPARISON_THRESHOLD) {
        double sum = 0.0;
        for (int i = 0; i < HOG_WIDTH_DIVISION * HOG_HEIGHT_DIVISION; i++) {
            sum += 1 - compareHist(partials[i], descriptor.partials[i], HISTCMP_BHATTACHARYYA);
        }
        return (full_compare + sum) / (HOG_WIDTH_DIVISION * HOG_HEIGHT_DIVISION + 1);
    }

    return full_compare;
}

Mat HistDescriptor::equalizeIntensity(const Mat &inputImage) {
    if (inputImage.channels() >= 3) {
        Mat ycrcb;

        cvtColor(inputImage, ycrcb, CV_HSV2BGR);
        cvtColor(ycrcb, ycrcb, CV_BGR2YCrCb);

        vector<Mat> channels;
        split(ycrcb, channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels, ycrcb);

        cvtColor(ycrcb, result, CV_YCrCb2BGR);

        return result;
    }
    return Mat();
}




