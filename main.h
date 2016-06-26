//
// Created by hya on 10.04.16.
//

#ifndef REIDENTIFICATION_MAIN_H
#define REIDENTIFICATION_MAIN_H

#define UI_FILE "gui.glade"

static const char *const VIDEO_PATH = "video/campus4-c0.avi";
static const char *const winname = "Press q to exit";
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
#include <gtk/gtk.h>
#include "UIFunctions.h"

using namespace cv;
using namespace cv::ml;

struct {
    double hitThreshold = 0;
    Size winStride = Size(8,8);
    Size padding = Size(32,32);
    double scale = 1.05;
    double finalThreshold = 2.0;
    bool useMeanShift = false;
} hogParams;

void start();

#endif //REIDENTIFICATION_MAIN_H
