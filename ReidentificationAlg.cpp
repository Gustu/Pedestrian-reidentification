//
// Created by hya on 26.06.16.
//

#include "ReidentificationAlg.h"

bool ReidentificationAlg::exit() {
    char key = (char) waitKey(30);
    switch (key) {
        case 'q':
        case 'Q':
        case 27: //escape key
            return true;
        default:
            return false;
    }
}

void ReidentificationAlg::processImage(ReidentificationData &data) {
//    resize(img, img, Size(0, 0), 2, 2, INTER_LANCZOS4);
    cvtColor(data.img, data.gray, COLOR_BGR2GRAY);
    getForegroundMask(data);
    cvtColor(data.clearMaskedImg, data.maskedGray, COLOR_BGR2GRAY);
    data.clearMaskedImg.copyTo(data.drawingImage);

    if (data.prevMaskedGray.empty())
        data.maskedGray.copyTo(data.prevMaskedGray);
}

void ReidentificationAlg::swapPoints(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        h->data.opticalFlow.swapPoints();
    }
}

void ReidentificationAlg::markAllAsLost(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        h->data.lostTracking = true;
    }
}

void ReidentificationAlg::calcOpticalFlows(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        if (!h->data.collision && !h->data.reinit) {
            h->data.opticalFlow.calculate(data.prevMaskedGray, data.maskedGray, h->data.boudingBox, h->data.outOfWindow);
        } else if (!h->data.collision && h->data.reinit) {
            h->data.opticalFlow.getOpticalFlowPoints(h->data.kalman.data.predRect, data.maskedGray);
            h->data.reinit = false;
        } else {
            h->data.opticalFlow.calculateWithCollision(h->data.move);
        }
        h->data.kalman.update(h->data.opticalFlow.data.boundingBox);
        if (h->data.opticalFlow.checkIfLostTracking(data.maskedGray)) {
            h->data.opticalFlow.reset();
        };
        if (h->data.kalman.data.lostTracking) {
            h->data.opticalFlow.reset();
            h->data.kalman.data.lostTracking = false;
        }
    }
}

void ReidentificationAlg::processKalmans(int width, int height, ReidentificationData &data) {
    getDt(data);
    for (Ptr<Human> h : data.identified) {
        if (h->data.kalman.data.notFoundCount > 100) {
            h->data.kalman.data.found = false;
            h->data.opticalFlow.data.blocked = true;
        }
        if (h->data.lostTracking) h->data.kalman.data.notFoundCount++;
        h->data.kalman.predict(data.dt);
        if (h->data.kalman.data.center.y < 0 || h->data.kalman.data.center.y > height || h->data.kalman.data.center.x < 0 ||
            h->data.kalman.data.center.x > width) {
            h->data.outOfWindow = true;
            h->data.opticalFlow.data.points->clear();
        }
        h->predictHumanPosition();
    }
}

void ReidentificationAlg::getDt(ReidentificationData &data) {
    double precTick = data.ticks;
    data.ticks = (double) getTickCount();
    data.dt = (data.ticks - precTick) / getTickFrequency(); //seconds
}

void ReidentificationAlg::applyAlgorithm(int frameId, ReidentificationData &data) {
    calcBeforeAfter(data.identified);
    calcCollisions(data.identified, data);
    calcOpticalFlows(data);
    processKalmans(data.img.cols, data.img.rows, data);

    if (frameId % data.detect_interval == 0) {
        data.hog.detectMultiScale(data.gray, data.foundLocations, data.foundWeights, hogParams.hitThreshold, hogParams.winStride,
                             hogParams.padding,
                             hogParams.scale, hogParams.finalThreshold, hogParams.useMeanShift);
        data.angleHog.detectMultiScale(data.gray, data.foundAngleLocations, data.foundAngleWeights, angleHogParams.hitThreshold,
                                  angleHogParams.winStride,
                                  angleHogParams.padding,
                                  angleHogParams.scale, angleHogParams.finalThreshold, angleHogParams.useMeanShiftGrouping);
        if (!data.foundAngleLocations.empty()) {
            for (vector<Rect>::iterator it = data.foundAngleLocations.begin(); it != data.foundAngleLocations.end(); ++it) {
                if (it != data.foundAngleLocations.end() && it != data.foundAngleLocations.begin() && isEmpty(data.maskedGray, *it, 25)) {
                    data.foundAngleLocations.erase(it);
                }
            }
        }

        for (int i = 0; i < data.foundLocations.size(); i++) {
            Rect trimmed = trimRect(data.foundLocations[i]);
            Ptr<Human> human = new Human();
            Ptr<Human> best = getBestMatch(human, trimmed, data);
            if (!best.empty()) {
                updateIdentified(trimmed, human, best, data);
            } else {
                newIdentified(trimmed, human, data);
            }
        }
    }

    drawing(data);

    swapPoints(data);

    cv::swap(data.prevMaskedGray, data.maskedGray);

    if (frameId % data.detect_interval == 0) {
        markAllAsLost(data);
    }
}

void ReidentificationAlg::calcBeforeAfter(vector<Ptr<Human>> identified) {
    for (Ptr<Human> h : identified) {
        h->calcBeforeAfter(h->data.point);
    }
}

Ptr<Human> ReidentificationAlg::getBestMatch(Ptr<Human> human, Rect rect, ReidentificationData &data) {
    Ptr<Human> bestHuman;
    human->data.histDescriptor.extractFeatures(data.img, rect, data.fgMaskMOG2);
    double best_comparison = 0;
    for (vector<Ptr<Human>>::iterator it = data.identified.begin(); it != data.identified.end(); it++) {
        double comparison = human->data.histDescriptor.compare(((*it)->data.histDescriptor));
        if (comparison > COMPARISON_THRESHOLD && best_comparison < comparison) {
            best_comparison = comparison;
            bestHuman = *it;
        }
    }

    if (bestHuman.empty()) {  // || isGlitch(human, bestHuman)
        return Ptr<Human>();
    }
    else {
        return bestHuman;
    }
}

bool ReidentificationAlg::isGlitch(Ptr<Human> human, Ptr<Human> bestMatch, ReidentificationData &data) {
    double dist = sqrt((human->data.point.x - bestMatch->data.point.x) ^ 2 + (human->data.point.y - bestMatch->data.point.y) ^ 2);
    return (bestMatch->data.move.x == 0 && bestMatch->data.move.y == 0) ?
           false :
           dist > 30.0 * sqrt((float) (bestMatch->data.move.x ^ 2 + bestMatch->data.move.y ^ 2));
}

void ReidentificationAlg::newIdentified(Rect &rect, Ptr<Human> &human, ReidentificationData &data) {
    human->data.id = data.id++;
    human->data.point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    human->data.color = randColor();
    human->data.boudingBox = rect;
    human->data.opticalFlow.getOpticalFlowPoints(rect, data.maskedGray);
    data.identified.push_back(human);
}


void ReidentificationAlg::updateIdentified(Rect &trimmed, Ptr<Human> &human,
                                           Ptr<Human> &best, ReidentificationData &data) {

    best->data.opticalFlow.data.blocked = false;
    best->data.boudingBox = trimmed;
    best->data.lostTracking = false;
    best->data.outOfWindow = false;
//
//    if (best->opticalFlow.points->empty()) {
//        best->opticalFlow.getOpticalFlowPoints(trimmed, maskedGray);
//    }

    best->data.point = Point(trimmed.x + trimmed.width / 2, trimmed.y + trimmed.height / 2);
    if (++best->data.histDescriptor.data.counter > HISTORY) {
        best->data.histDescriptor = human->data.histDescriptor;
        best->data.gaborDescriptor = human->data.gaborDescriptor;
        best->data.histDescriptor.data.counter = 0;
    }

    if (!best->data.opticalFlow.data.centroid.inside(best->data.boudingBox)) {
        best->data.opticalFlow.getOpticalFlowPoints(best->data.boudingBox, data.maskedGray);
    }

    best->data.kalman.resetCounter();
    best->data.kalman.update(trimmed);
}

void ReidentificationAlg::drawing(ReidentificationData &data) {
    draw_detections(data.drawingImage, data.foundAngleLocations);
//    draw_identified(drawingImage, identified);
//    draw_kalmans(drawingImage, identified);
//    draw_points(drawingImage, identified);


    imshow(winname, data.drawingImage);
}

void ReidentificationAlg::calcCollisions(vector<Ptr<Human>> identified, ReidentificationData &data) {
    for_each(identified.begin(), identified.end(), [](Ptr<Human> &human) { human->data.collision = false; });
    for (Ptr<Human> h1 : identified) {
        if (!h1->data.outOfWindow) {
            for (Ptr<Human> h2 : identified) {
                if (h1->data.id != h2->data.id && !h2->data.outOfWindow) {
                    Rect intersect = h1->data.kalman.data.predRect & h2->data.kalman.data.predRect;
                    if (intersect.width > 0 && intersect.height > 0) {
                        rectangle(data.drawingImage, intersect, Scalar(0, 0, 255));
                        h1->data.collision = true;
                        h2->data.collision = true;
                        h1->data.reinit = true;
                        h2->data.reinit = true;
                    }
                }
            }
        }
    }
}

void ReidentificationAlg::draw_kalmans(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> h: identified) {
        if (h->data.kalman.data.found) {
            h->data.kalman.drawPredicted(img);
        }
    }
}

void ReidentificationAlg::draw_points(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> h : identified) {
        h->data.opticalFlow.drawPoints(img, h->data.color);
    }
}

void ReidentificationAlg::init(ReidentificationData &data) {
    namedWindow(winname, WINDOW_KEEPRATIO);
    data.hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
#ifndef RETRAIN_HOG
    data.hogTrainer = new HOGTrainer(Size(64, 128));
#else
    hogTrainer = new HOGTrainer("hog/pos/", "hog/pos.list", "hog/neg/", "hog/neg.list", Size(64, 128));
#endif
    data.angleHog = data.hogTrainer->getHOG();
    data.pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    data.cap = new VideoCapture(data.fileName);
}

void ReidentificationAlg::getForegroundMask(ReidentificationData &data) {
    int morph_elem = 2;
    data.pMOG2->apply(data.img, data.fgMaskMOG2, 0);
    // Since MORPH_X : 2,3,4,5 and 6

    medianBlur(data.fgMaskMOG2, data.fgMaskMOG2, 2 * 2 + 1);

    int morph_size2 = 3;

    morphologyEx(data.fgMaskMOG2, data.fgMaskMOG2, 3,
                 getStructuringElement(morph_elem, Size(2 * morph_size2 + 1, 2 * morph_size2 + 1),
                                       Point(morph_size2, morph_size2)));

    data.clearMaskedImg.release();
    data.img.copyTo(data.clearMaskedImg, data.fgMaskMOG2);
}

Rect ReidentificationAlg::trimRect(Rect rect) {
    int pad_w = (int) (0.15 * rect.width);
    int pad_h = (int) (0.05 * rect.height);
    Point pt1(rect.x + pad_w, rect.y + pad_h);
    Point pt2(rect.x + rect.width - pad_w, rect.y + rect.height - pad_h);
    return Rect(pt1, pt2);
}

void ReidentificationAlg::draw_detections(Mat frame, vector<Rect> rects) {
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

void ReidentificationAlg::draw_identified(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> human : identified) {
        if (human->data.kalman.data.found) {
            string str = "ID:";
            str.append(to_string(human->data.id));

            putText(img, str, human->data.predicted, CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
            circle(img, human->data.predicted, 4, human->data.color);
        }
    }
}

void ReidentificationAlg::start(ReidentificationData &data) {
    int frame_idx = 0;
    while (data.cap->isOpened()) {
        if (data.exiting) {
            break;
        }
        data.cap->read(data.img);
        if (data.img.empty()) {
            break;
        }

        processImage(data);

        applyAlgorithm(frame_idx, data);

        frame_idx++;

        if (exit()) break;
    }

    data.cap->release();
    data.fgMaskMOG2.release();
    data.prevMaskedGray.release();
    data.maskedGray.release();
    data.gray.release();
    data.img.release();
    data.foundLocations.clear();
    data.foundWeights.clear();
    data.identified.clear();
    data.drawingImage.release();
    data.clearMaskedImg.release();
    data.id = 0;
    data.dt = 0;
    data.ticks = 0;
    data.exiting = false;

    destroyWindow(winname);

    delete data.cap;
}

cv::Scalar ReidentificationAlg::randColor() {
    return Scalar(rand() % 256, rand() % 256, rand() % 256);
}

void ReidentificationAlg::setFileName(char *fileName, ReidentificationData &data) {
    data.fileName = fileName;
}

void ReidentificationAlg::stop(ReidentificationData &data) {
    data.exiting = true;
}

ReidentificationAlg::ReidentificationAlg(char *fileName, ReidentificationData &data) {
    data.fileName = fileName;
    data.exiting = false;
//    this->haarDetector.load();
}

ReidentificationAlg::ReidentificationAlg(ReidentificationData &data) {
    data.exiting = false;
//    this->haarDetector.load();
}

void ReidentificationAlg::detectFaces(Mat &img, ReidentificationData &data) {
    vector<Rect> faces = data.haarDetector.detectFaces(img);
    for (Rect face: faces) {
        rectangle(data.drawingImage, face, Scalar(255, 0, 255), 3);
    }
}

bool ReidentificationAlg::isEmpty(Mat &img, Rect &rect, int density) {
    if (img.empty()) {
        return true;
    }
    int count = 0;
    if (rect.width <= 0 || rect.height <= 0 || rect.x + rect.width > img.cols || rect.x < 0 ||
        rect.y + rect.height > img.rows || rect.y < 0) {
        return true;
    }
    for (int i = 0; i < density * density; i++) {
        Point p(rand() % rect.width, rand() % rect.height);
        Vec3f c = img.at<Vec3f>(p);
        if (c(0) == 0 && c(1) == 0 && c(2) == 0) {
            count++;
        }
    }
    return (double) count / (density * density) > EMPTY_LIMIT;
}











