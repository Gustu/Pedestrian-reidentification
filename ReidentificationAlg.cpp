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

void ReidentificationAlg::processImage() {
//    resize(img, img, Size(0, 0), 2, 2, INTER_LANCZOS4);
    cvtColor(img, gray, COLOR_BGR2GRAY);
    getForegroundMask(img, fgMaskMOG2);
    cvtColor(clearMaskedImg, maskedGray, COLOR_BGR2GRAY);
    clearMaskedImg.copyTo(drawingImage);

    if (prevMaskedGray.empty())
        maskedGray.copyTo(prevMaskedGray);
}

void ReidentificationAlg::swapPoints() {
    for (Ptr<Human> h : identified) {
        h->opticalFlow.swapPoints();
    }
}

void ReidentificationAlg::markAllAsLost() {
    for (Ptr<Human> h : identified) {
        h->lostTracking = true;
    }
}

void ReidentificationAlg::calcOpticalFlows() {
    for (Ptr<Human> h : identified) {
        if (!h->collision && !h->reinit) {
            h->opticalFlow.calculate(prevMaskedGray, maskedGray, h->boudingBox, h->outOfWindow);
        } else if (!h->collision && h->reinit) {
            h->opticalFlow.getOpticalFlowPoints(h->kalman.predRect, maskedGray);
            h->reinit = false;
        } else {
            h->opticalFlow.calculateWithCollision(h->move);
        }
        h->kalman.update(h->opticalFlow.boundingBox);
        h->kalman.checkIfLostTracking(maskedGray);
        if (h->kalman.lostTracking) {
            h->opticalFlow.reset();
            h->kalman.lostTracking = false;
        }
    }
}

void ReidentificationAlg::processKalmans(int width, int height) {
    getDt();
    for (Ptr<Human> h : identified) {
        if (h->kalman.notFoundCount > 100) {
            h->kalman.found = false;
            h->opticalFlow.blocked = true;
        }
        if (h->lostTracking) h->kalman.notFoundCount++;
        h->kalman.predict(dt);
        if (h->kalman.center.y < 0 || h->kalman.center.y > height || h->kalman.center.x < 0 ||
            h->kalman.center.x > width) {
            h->outOfWindow = true;
            h->opticalFlow.points->clear();
        }
        h->predictHumanPosition();
    }
}

void ReidentificationAlg::getDt() {
    double precTick = ticks;
    ticks = (double) getTickCount();
    dt = (ticks - precTick) / getTickFrequency(); //seconds
}

void ReidentificationAlg::applyAlgorithm(int frameId) {
    calcBeforeAfter(identified);
    calcCollisions(identified);
    calcOpticalFlows();
    processKalmans(img.cols, img.rows);

    if (frameId % detect_interval == 0) {
        hog.detectMultiScale(gray, foundLocations, foundWeights, hogParams.hitThreshold, hogParams.winStride,
                             hogParams.padding,
                             hogParams.scale, hogParams.finalThreshold, hogParams.useMeanShift);
        for (int i = 0; i < foundLocations.size(); i++) {
            Rect trimmed = trimRect(foundLocations[i]);
            Ptr<Human> human = new Human();
            Ptr<Human> best = getBestMatch(human, trimmed);
            if (!best.empty()) {
                updateIdentified(trimmed, human, best);
            } else {
                newIdentified(trimmed, human);
            }
        }
    }

    drawing();

    swapPoints();

    cv::swap(prevMaskedGray, maskedGray);

    if (frameId % detect_interval == 0) {
        markAllAsLost();
    }
}

void ReidentificationAlg::calcBeforeAfter(vector<Ptr<Human>> identified) {
    for (Ptr<Human> h : identified) {
        h->calcBeforeAfter(h->point);
    }
}

Ptr<Human> ReidentificationAlg::getBestMatch(Ptr<Human> human, Rect rect) {
    Ptr<Human> bestHuman;
    human->histDescriptor.extractFeatures(img, rect, fgMaskMOG2);
    double best_comparison = 0;
    for (vector<Ptr<Human>>::iterator it = identified.begin(); it != identified.end(); it++) {
        double comparison = human->histDescriptor.compare(((*it)->histDescriptor));
        if (comparison > COMPARISON_THRESHOLD && best_comparison < comparison) {
            best_comparison = comparison;
            bestHuman = *it;
        }
    }

    if (bestHuman.empty() || isGlitch(human, bestHuman)) {
        return Ptr<Human>();
    }
    else {
        return bestHuman;
    }
}

bool ReidentificationAlg::isGlitch(Ptr<Human> human, Ptr<Human> bestMatch) {
    double dist = sqrt((human->point.x - bestMatch->point.x) ^ 2 + (human->point.y - bestMatch->point.y) ^ 2);
    return (bestMatch->move.x == 0 && bestMatch->move.y == 0) ?
           false :
           dist > 30.0 * sqrt((float) (bestMatch->move.x ^ 2 + bestMatch->move.y ^ 2));
}

void ReidentificationAlg::newIdentified(Rect &rect, Ptr<Human> &human) {
    human->id = id++;
    human->point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    human->color = randColor();
    human->boudingBox = rect;
    human->opticalFlow.getOpticalFlowPoints(rect, maskedGray);
    identified.push_back(human);
}


void ReidentificationAlg::updateIdentified(Rect &trimmed, Ptr<Human> &human,
                      Ptr<Human> &best) {

    best->opticalFlow.blocked = false;
    best->boudingBox = trimmed;
    best->lostTracking = false;
    best->outOfWindow = false;

    if (best->opticalFlow.points->empty()) {
        best->opticalFlow.getOpticalFlowPoints(trimmed, maskedGray);
    }

    best->point = Point(trimmed.x + trimmed.width / 2, trimmed.y + trimmed.height / 2);
    if (++best->histDescriptor.counter > HISTORY) {
        best->histDescriptor = human->histDescriptor;
        best->gaborDescriptor = human->gaborDescriptor;
        best->histDescriptor.counter = 0;
    }

    if (!best->opticalFlow.centroid.inside(best->boudingBox)) {
        best->opticalFlow.getOpticalFlowPoints(best->boudingBox, maskedGray);
    }

    best->kalman.resetCounter();
    best->kalman.update(trimmed);
}

void ReidentificationAlg::drawing() {
    draw_detections(drawingImage, foundLocations);
    draw_identified(drawingImage, identified);
    draw_kalmans(drawingImage, identified);
    draw_points(drawingImage, identified);


    imshow(winname, drawingImage);
}

void ReidentificationAlg::calcCollisions(vector<Ptr<Human>> identified) {
    for_each(identified.begin(), identified.end(), [](Ptr<Human> &human) { human->collision = false; });
    for (Ptr<Human> h1 : identified) {
        if (!h1->outOfWindow) {
            for (Ptr<Human> h2 : identified) {
                if (h1->id != h2->id && !h2->outOfWindow) {
                    Rect intersect = h1->kalman.predRect & h2->kalman.predRect;
                    if (intersect.width > 0 && intersect.height > 0) {
                        h1->collision = true;
                        h2->collision = true;
                        h1->reinit = true;
                        h2->reinit = true;
                    }
                }
            }
        }
    }
}

void ReidentificationAlg::draw_kalmans(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> h: identified) {
        if (h->kalman.found) {
            h->kalman.drawPredicted(img);
        }
    }
}

void ReidentificationAlg::draw_points(Mat img, vector<Ptr<Human>> identified) {
    for (Ptr<Human> h : identified) {
        h->opticalFlow.drawPoints(img, h->color);
    }
}

void ReidentificationAlg::init() {
    namedWindow(winname, WINDOW_KEEPRATIO);
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    cap = new VideoCapture(fileName);
}

void ReidentificationAlg::getForegroundMask(const Mat &curImage, Mat &mask) {
    int morph_elem = 2;
    pMOG2->apply(curImage, mask, 0);
    // Since MORPH_X : 2,3,4,5 and 6

    medianBlur(mask, mask, 2 * 2 + 1);

    int morph_size2 = 3;

    morphologyEx(mask, mask, 3,
                 getStructuringElement(morph_elem, Size(2 * morph_size2 + 1, 2 * morph_size2 + 1),
                                       Point(morph_size2, morph_size2)));

    clearMaskedImg.release();
    curImage.copyTo(clearMaskedImg, fgMaskMOG2);
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
        if (human->kalman.found) {
            string str = "ID:";
            str.append(to_string(human->id));

            putText(img, str, human->predicted, CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
            circle(img, human->predicted, 4, human->color);
        }
    }
}

void ReidentificationAlg::start() {
    init();
    int frame_idx = 0;
    while (cap->isOpened()) {
        cap->read(img);
        if (img.empty()) {
            break;
        }

        processImage();

        applyAlgorithm(frame_idx);

        frame_idx++;

        if (exit()) break;
    }

    delete cap;
}

cv::Scalar ReidentificationAlg::randColor() {
    return Scalar(rand() % 256, rand() % 256, rand() % 256);
}

void ReidentificationAlg::setFileName(char *fileName) {
    this->fileName = fileName;
}

