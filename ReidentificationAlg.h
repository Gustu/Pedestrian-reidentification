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
#include "View.h"
#include <iostream>

#define EMPTY_LIMIT 0.3

using namespace cv;
using namespace cv::ml;

//#define RETRAIN_HOG

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
    ReidentificationAlg() {}

    Rect trimRect(Rect &r);

    void getForegroundMask(ReidentificationData &data);

    void draw_identified(Mat &img, vector<Ptr<Human>> &identified);

    void draw_detections(Mat &frame, vector<Rect> &rects);

    void drawing(ReidentificationData &data, String &winname);

    void updateIdentified(Rect &trimmed, Ptr<Human> &human, Ptr<Human> &best, ReidentificationData &data);

    void newIdentified(Rect &rect, Ptr<Human> &human, ReidentificationData &data);

    Ptr<Human> getBestMatch(Ptr<Human> &human, Rect &rect_, vector<Ptr<Human>> &identified, ReidentificationData &data);

    void applyAlgorithm(vector<View> &views);

    void draw_points(Mat &img, vector<Ptr<Human>> &identified);

    void getDt(ReidentificationData &data);

    void processKalmans(int &i, int &i1, ReidentificationData &data);

    void calcOpticalFlows(ReidentificationData &data);

    void markAllAsLost(ReidentificationData &data);

    void draw_kalmans(Mat &img, vector<Ptr<Human>> &identified);

    void swap(ReidentificationData &data);

    void processImage(ReidentificationData &data);

    bool exit();

    void calcCollisions(vector<Ptr<Human>> &identified, ReidentificationData &data);

    void calcBeforeAfter(vector<Ptr<Human>> &identified);

    bool isGlitch(Ptr<Human> &ptr, Ptr<Human> &ptr1);

    void stop(ReidentificationData &data);

    Scalar randColor();

    void setFileName(char *fileName, ReidentificationData &data);

    void detectFaces(Mat &img, ReidentificationData &data);

    bool isEmpty(Mat &img, Rect &rect, int density);

    void clear(ReidentificationData &data);

    void finish(ReidentificationData &data, const String &winname);

    int getIndexIfHumanExists(vector<Ptr<Human>> identified, int id);

    void copyIdentified(Rect &rect, Ptr<Human> &human, ReidentificationData &data);

    bool isGlitch(Rect &rect1, Rect &rect2);
};


#endif //REIDENTIFICATION_ALGORITHM_H
