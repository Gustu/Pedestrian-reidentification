//
// Created by hya on 10.04.16.
//

#ifndef REIDENTIFICATION_MAIN_H
#define REIDENTIFICATION_MAIN_H

using namespace std;

#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace cv;

struct {
    int hitThreshold = 0;
    Size winStride = Size(8,8);
    Size padding = Size(32,32);
    double scale = 1.05;
    double finalThreshold = 2.0;
    bool useMeanShift = false;
} hogParams;

struct optical_flow_parameters {
    Size winSize = Size(15,15);
    int maxLevel = 2;
    TermCriteria termcrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
} opticalFlowParameters;

struct feature_params {
    int maxCorners = 500;
    int qualitLevel = 0.3;
    int minDistance = 7;
    int blockSize = 7;
} featureParams;

#endif //REIDENTIFICATION_MAIN_H
