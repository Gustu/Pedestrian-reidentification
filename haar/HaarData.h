//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_HAARDATA_H
#define REIDENTIFICATION_HAARDATA_H

#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

class HaarData {
public:
    CascadeClassifier *face_cascade;
};


#endif //REIDENTIFICATION_HAARDATA_H
