#include <iostream>
#include "main.h"
#include "Human.h"

using namespace std;
using namespace cv;

int detect_interval = 5;
int id = 0;
HOGDescriptor hog;

void drawPolylines(cv::Mat &img, const list<Point> &points, Scalar color) {
    list<cv::Point_<int>>::const_iterator current = points.begin();
    for (list<cv::Point_<int>>::const_iterator it = points.begin(); it != points.end(); it++) {
        line(img, *current, *it, color);
        current = it;
    }
}

void draw_identified(Mat img, vector<Human> identified) {
    for (Human human : identified) {
        string str = "ID:";
        str.append(to_string(human.id));

        putText(img, str, human.point, CV_FONT_HERSHEY_PLAIN, 1, human.color, 1);
        circle(img, human.point, 4, human.color);
        drawPolylines(img, human.track, human.color);
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
    namedWindow(window_name, WINDOW_KEEPRATIO); //resizable window;

    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    VideoCapture *cap = new VideoCapture("/home/hya/workspace/reidentification/video/campus4-c0.avi");
    Mat gray, prevGray, image, frame;
    vector<Rect> foundLocations;
    vector<Point2f> points[2];

    int frame_idx = 0;
    vector<Human> identified;
//    identified.push_back(Human(Point(30, 30)));

    while (cap->isOpened()) {
        cap->read(frame);
        if (frame.empty()) {
            break;
        }

        if (frame_idx % detect_interval == 0) {
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            hog.detectMultiScale(frame, foundLocations, hogParams.hitThreshold, hogParams.winStride, hogParams.padding,
                                 hogParams.scale, hogParams.finalThreshold, hogParams.useMeanShift);

            for (Rect rect : foundLocations) {
                bool is = false;
                for (int i = 0; i < identified.size(); i++) {
                    if (identified[i].point.inside(rect)) {
                        identified[i].point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
                        identified[i].addToTrack(identified[i].point);
                        identified[i].locked = true;
                        is = true;
                        break;
                    }
                }
                if (!is) {
                    identified.push_back(
                            Human(id++, Point(rect.x + rect.width / 2, rect.y + rect.height / 2), randColor()));
                }
            }

            for (vector<Human>::iterator it = identified.begin(); it != identified.end();) {
                if(!(*it).locked) {
                    it = identified.erase(it);
                } else {
                    (*it).locked = false;
                    ++it;
                }
            }
        }



        draw_detections(frame, foundLocations);
        draw_identified(frame, identified);
        imshow(window_name, frame);

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

    return 0;
}

