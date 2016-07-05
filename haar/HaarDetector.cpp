//
// Created by hya on 02.07.16.
//

#include "HaarDetector.h"

HaarDetector::HaarDetector() {
    data.face_cascade = new CascadeClassifier();
}

vector<Rect> HaarDetector::detectFaces(Mat &img) {
    vector<Rect> faces;
    if (!img.empty()) {
        Mat resized;
        resize(img, resized, Size(0, 0), 3, 3, CV_INTER_LANCZOS4);
        data.face_cascade->detectMultiScale(img, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(20, 20));
        resizeFaces(faces, 3, resized.cols, resized.rows);
    }
    return faces;
}

void HaarDetector::load() {
    if (!data.face_cascade->load("/home/hya/workspace/reidentification/haar/data/haarcascade_frontalface_default.xml")) {
        printf("--(!)Error loading\n");
    };
}

void HaarDetector::resizeFaces(vector<Rect> &faces, int scale, int resizedWidth, int resizedHeight) {
    for (Rect &face : faces) {
        face.width /= scale;
        face.height /= scale;
        face.x -= resizedWidth - resizedWidth/3;
        face.y -= resizedHeight - resizedHeight/3;
    }
}







