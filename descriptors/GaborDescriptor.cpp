//
// Created by hya on 15.05.16.
//

#include "GaborDescriptor.h"

void GaborDescriptor::extractFeatures(Mat img, Rect rect) {
    Mat ycrcb;
    Mat channels[3];
    cvtColor(img, ycrcb, COLOR_BGR2YCrCb);

    split(ycrcb, channels);
    Mat roi(channels[0], rect);
    resize(roi, roi, Size(128, 256), 0, 0, INTER_LANCZOS4);
    data.means.clear();
    data.stdevs.clear();
    for (Mat kernel : data.filterBank.getInstance().kernels) {
        Mat dest;
        filter2D(roi, dest, CV_32F, kernel);
        vector<double> mean;
        vector<double> stdev;
        meanStdDev(dest, mean, stdev);
        data.means.push_back(mean[0]);
        data.stdevs.push_back(stdev[0]);
    }
}

double GaborDescriptor::compare(GaborDescriptor descriptor) {
    double sum = 0.0;
    for(int i = 0 ; i < data.means.size(); i++) {
        sum += abs(data.means[i] - descriptor.data.means[i]) + abs(data.stdevs[i] - descriptor.data.stdevs[i]);
    }
    return sum;
}



