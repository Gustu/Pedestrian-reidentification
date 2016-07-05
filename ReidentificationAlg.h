//
// Created by hya on 26.06.16.
//

#ifndef REIDENTIFICATION_ALGORITHM_H
#define REIDENTIFICATION_ALGORITHM_H

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
#include "models/Human.h"
#include "haar/HaarDetector.h"
#include "HOGTrainer.h"
#include "ReidentificationData.h"
#include <iostream>

#define EMPTY_LIMIT 0.3

using namespace cv;
using namespace cv::ml;

//#define RETRAIN_HOG

const char *const winname = "Press q to exit";

struct {
    double hitThreshold = 0;
    Size winStride = Size(8, 8);
    Size padding = Size(32, 32);
    double scale = 1.05;
    double finalThreshold = 2.0;
    bool useMeanShift = false;
} hogParams;

struct {
    double hitThreshold = 0;
    Size winStride = Size(8, 8);
    Size padding = Size(32, 32);
    double scale = 1.05;
    double finalThreshold = 2.0;
    bool useMeanShiftGrouping = true;
} angleHogParams;


class ReidentificationAlg {
public:
    ReidentificationData data;

    ReidentificationAlg();

    ReidentificationAlg(char *fileName);

    Rect trimRect(Rect r);

    void getForegroundMask(const Mat &curImage, Mat &mask);

    void draw_identified(Mat img, vector<Ptr<Human>> identified);

    void draw_detections(Mat frame, vector<Rect> rects);

    void init();

    void drawing();

    void updateIdentified(Rect &trimmed, Ptr<Human> &human, Ptr<Human> &best);

    void newIdentified(Rect &rect, Ptr<Human> &human);

    Ptr<Human> getBestMatch(Ptr<Human> human, Rect rect_);

    void applyAlgorithm(int frameId);

    void draw_points(Mat img, vector<Ptr<Human>> identified);

    void getDt();

    void processKalmans(int i, int i1);

    void calcOpticalFlows();

    void markAllAsLost();

    void draw_kalmans(Mat img, vector<Ptr<Human>> identified);

    void swapPoints();

    void processImage();

    bool exit();

    void calcCollisions(vector<Ptr<Human>> identified);

    void calcBeforeAfter(vector<Ptr<Human>> identified);

    bool isGlitch(Ptr<Human> ptr, Ptr<Human> ptr1);

    void start();

    void stop();

    Scalar randColor();

    void setFileName(char *fileName);

    void detectFaces(Mat &img);

    bool isEmpty(Mat &img, Rect &rect, int density);
};


#endif //REIDENTIFICATION_ALGORITHM_H
