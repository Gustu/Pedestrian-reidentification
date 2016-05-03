#include <iostream>
#include "main.h"
#include "Human.h"
#include "descriptors/KMeansDescriptor.h"

using namespace std;
using namespace cv;

int detect_interval = 5;
int id = 0;
HOGDescriptor hog;

Rect trimRect(Rect r);

void updateIdentified(const vector<Rect> &foundLocations, vector<Human> &identified, int j);

void eraseLost(vector<Human> &identified, vector<Human>::iterator &it);

void kmeansCluster(const Mat &img, const Rect &roi);

void get_svm_detector(const Ptr<SVM> &svm, vector<float> &hog_detector) {
    // get the support vectors
    Mat sv = svm->getSupportVectors();
    const int sv_total = sv.rows;
    // get the decision function
    Mat alpha, svidx;
    double rho = svm->getDecisionFunction(0, alpha, svidx);

    CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
    CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
              (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
    CV_Assert(sv.type() == CV_32F);
    hog_detector.clear();

    hog_detector.resize(sv.cols + 1);
    memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));
    hog_detector[sv.cols] = (float) -rho;
}

void draw_identified(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> human : identified) {
        string str = "ID:";
        str.append(to_string(human->id));

        putText(img, str, human->point, CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
        circle(img, human->point, 4, human->color);
    }
}

void draw_detections(Mat frame, vector<Rect> rects) {
    for (Rect rect : rects) {
        int pad_w = (int) (0.15 * rect.width);
        int pad_h = (int) (0.05 * rect.height);
        Point pt1(rect.x + pad_w, rect.y + pad_h);
        Point pt2(rect.x + rect.width - pad_w, rect.y + rect.height - pad_h);

        putText(frame, "Human", pt1, CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
        rectangle(frame, pt1, pt2, Scalar(0, 255, 0));
        circle(frame, Point(rect.x + rect.width / 2, rect.y + rect.height / 2), 3, Scalar(0, 0, 255));
    }
}

cv::Scalar randColor() {
    return Scalar(rand() % 256, rand() % 256, rand() % 256);
}

int main() {
    string window_name = "video | q or esc to quit";
    namedWindow(window_name, WINDOW_KEEPRATIO); //resizable window
//    Ptr<SVM> svm = StatModel::load<SVM>("/home/hya/workspace/reidentification/my_people_detector.yml");
//    // Set the trained svm to my_hog
//    vector<float> hog_detector;
//    get_svm_detector(svm, hog_detector);
//    hog.winSize = Size(96, 160);
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    VideoCapture *cap = new VideoCapture("/home/hya/workspace/reidentification/video/campus4-c0.avi");
    Mat gray, curImage;
    vector<Rect> foundLocations;
    vector<double> foundWeights;
    vector<Point2f> points[2];

    int frame_idx = 0;
    vector<Ptr<Human>> identified;
    Mat m;

    Mat bgImage;
    Mat diffImage;
    int morph_elem = 2;
    int morph_size = 3;

    while (cap->isOpened()) {
        cap->read(curImage);
        if (curImage.empty()) {
            break;
        }
        if (bgImage.empty()) {
            curImage.copyTo(bgImage);
        }
        absdiff(bgImage, curImage, diffImage);
        Mat mask = Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);


        float threshold = 10.0f;
        float dist;
        for (int j = 0; j < diffImage.rows; ++j) {
            for (int i = 0; i < diffImage.cols; ++i) {
                cv::Vec3b pix = diffImage.at<cv::Vec3b>(j, i);

                dist = (pix[0] * pix[0] + pix[1] * pix[1] + pix[2] * pix[2]);
                dist = sqrt(dist);

                if(dist > threshold ) {
                    mask.at<uchar>(j,i) = 255;

                }

            }
        }

        // Since MORPH_X : 2,3,4,5 and 6
        int operation = 2;

        Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

        /// Apply the specified morphology operation
        morphologyEx(mask, mask, operation, element );
        int dilation_size = 3;

        dilate(mask, mask, getStructuringElement( 0,
                                                                     Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                                                     Point( dilation_size, dilation_size ) ));
        curImage.copyTo(mask, mask);

//        if (frame_idx % detect_interval == 0) {
//            cvtColor(curImage, gray, COLOR_BGR2GRAY);
//
//            hog.detectMultiScale(gray, foundLocations, foundWeights, hogParams.hitThreshold, hogParams.winStride,
//                                 hogParams.padding,
//                                 hogParams.scale, hogParams.finalThreshold, hogParams.useMeanShift);
//
//            for(Rect rect: foundLocations) {
//                Ptr<Human> human = new Human();
//                Mat h(mask, rect);
//                human->descriptor.extractDescriptor(h);
//                bool matched = false;
//                for(vector<Ptr<Human>>::iterator it = identified.begin(); it != identified.end(); it++) {
//                    float comparison = human->descriptor.compare(&((*it)->descriptor), 0.5);
//                    if(comparison > 0.6) {
//                        matched = true;
//                        (*it)->point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
//                        break;
//                    }
//                }
//                if(!matched) {
//                    human->id = id++;
//                    human->point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
//                    identified.push_back(human);
//                }
//            }
//        }


//        draw_detections(curImage, foundLocations);
        draw_identified(mask, identified);
        Mat changed;
        Mat *channels;
        cvtColor(curImage, changed, COLOR_BGR2Luv);
        split(changed, channels);

        imshow(window_name, channels[0]);

        frame_idx++;

        char key = (char) waitKey(30);
        switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                return 0;
            default:
                break;
        }
    }


    delete cap;

    return 0;
}

Rect trimRect(Rect rect) {
    int pad_w = (int) (0.15 * rect.width);
    int pad_h = (int) (0.05 * rect.height);
    Point pt1(rect.x + pad_w, rect.y + pad_h);
    Point pt2(rect.x + rect.width - pad_w, rect.y + rect.height - pad_h);
    return Rect(pt1, pt2);
}



