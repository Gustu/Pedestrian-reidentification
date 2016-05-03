//
// Created by hya on 24.04.16.
//

#ifndef REIDENTIFICATION_DESCRIPTOR_H
#define REIDENTIFICATION_DESCRIPTOR_H

#define CLUSTER_COUNT 7

static const double BLACK_COLOR = 1;

#include <cv.hpp>
#include <opencv2/core/mat.hpp>
#include <map>
#include "../Color.h"

using namespace cv;
using namespace std;

class KMeansDescriptor{

public:
    vector<Ptr<Color>> colors;
    void extractDescriptor(Mat src, int attempts = 10);
    float compare(KMeansDescriptor *descr, float tolerance = 0.2);
};


#endif //REIDENTIFICATION_DESCRIPTOR_H
