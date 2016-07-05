//
// Created by hya on 02.07.16.
//

#ifndef REIDENTIFICATION_HAARDETECTOR_H
#define REIDENTIFICATION_HAARDETECTOR_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

class HaarDetector {
public:
    CascadeClassifier *face_cascade;

    HaarDetector();

    vector<Rect> detectFaces(Mat &img);
    void load();
private:
    void resizeFaces(vector<Rect> &faces, int scale, int resizedWidth, int resizedHeight);
};


#endif //REIDENTIFICATION_HAARDETECTOR_H
