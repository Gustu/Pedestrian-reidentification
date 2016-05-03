//
// Created by hya on 24.04.16.
//
#include "KMeansDescriptor.h"

void KMeansDescriptor::extractDescriptor(Mat src, int attempts) {
    Mat samples(src.rows * src.cols, 3, CV_32F);
    for (int y = 0; y < src.rows; y++)
        for (int x = 0; x < src.cols; x++)
            for (int z = 0; z < 3; z++)
                samples.at<float>(y + x * src.rows, z) = src.at<Vec3b>(y, x)[z];

    Mat labels;
    Mat centers;
    kmeans(samples, CLUSTER_COUNT, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts,
           KMEANS_PP_CENTERS, centers);

    vector<float> hist(centers.rows);
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            int cluster_idx = labels.at<int>(y + x * src.rows, 0);
            hist.at(cluster_idx)++;
        }
    }
    float fix_scale = 0;
    for (int i = 0; i < hist.size(); i++) {
        if(centers.at<Vec3f>(i)[0] < BLACK_COLOR && centers.at<Vec3f>(i)[1] < BLACK_COLOR && centers.at<Vec3f>(i)[0] <
                                                                                             BLACK_COLOR) {
            fix_scale = hist.at(i) / (labels.rows * labels.cols);
        }
    }
    vector<Ptr<Color>>::iterator it;
    for (int i = 0; i < hist.size(); i++) {
        it = colors.begin();
        hist.at(i) = hist.at(i) / (labels.rows * labels.cols) / fix_scale == 0 ? 1 : fix_scale;
        for (; it != colors.end(); it++) {
            if ((*it)->probability < hist.at(i)) {
                break;
            }
        }
        if (centers.at<Vec3f>(i)[0] > BLACK_COLOR && centers.at<Vec3f>(i)[1] > BLACK_COLOR && centers.at<Vec3f>(i)[0] >
                                                                                              BLACK_COLOR) {
            colors.insert(it, new Color(hist.at(i), centers.at<Vec3f>(i)));
        }
    }
}

float KMeansDescriptor::compare(KMeansDescriptor *descr, float tolerance) {
    float sum = 0.0;
    for (int i = 0; i < colors.size(); i++) {
        float comparison = colors[i]->compare(descr->colors[i]) / 255;
        sum += (comparison > (1 - tolerance)) ? (colors[i]->probability + descr->colors[i]->probability) : 0.0;
    }
    return sum;
}



