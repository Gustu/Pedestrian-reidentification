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

void ReidentificationAlg::swap(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        h->data.opticalFlow.swapPoints();
    }
    cv::swap(data.prevMaskedGray, data.maskedGray);
}

void ReidentificationAlg::markAllAsLost(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        h->data.lostTracking = true;
    }
}

void ReidentificationAlg::calcOpticalFlows(ReidentificationData &data) {
    for (Ptr<Human> h : data.identified) {
        if (!h->data.collision && !h->data.reinit) {
            h->data.opticalFlow.calculate(data.prevMaskedGray, data.maskedGray, h->data.boudingBox,
                                          h->data.outOfWindow);
        } else if (!h->data.collision && h->data.reinit) {
            h->data.opticalFlow.getOpticalFlowPoints(h->data.kalman.data.predRect, data.maskedGray);
            h->data.reinit = false;
        } else {
            h->data.opticalFlow.calculateWithCollision(h->data.move);
        }
        h->data.kalman.update(h->data.opticalFlow.data.boundingBox);
        if (h->data.opticalFlow.isTrackingLost(data.maskedGray)) {
            h->data.opticalFlow.reset();
        };
        if (h->data.kalman.data.lostTracking) {
            h->data.opticalFlow.reset();
            h->data.kalman.data.lostTracking = false;
        }
    }
}

void ReidentificationAlg::processKalmans(int &width, int &height, ReidentificationData &data) {
    getDt(data);
    for (Ptr<Human> h : data.identified) {
        if (h->data.kalman.data.notFoundCount > 100) {
            h->data.kalman.data.found = false;
            h->data.opticalFlow.data.blocked = true;
        }
        if (h->data.lostTracking) h->data.kalman.data.notFoundCount++;
        h->data.kalman.predict(data.dt);
        if (h->data.kalman.data.center.y < 0 || h->data.kalman.data.center.y > height ||
            h->data.kalman.data.center.x < 0 ||
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

void ReidentificationAlg::applyAlgorithm(vector<View> &views) {
    for (vector<View>::iterator it = views.begin(); it != views.end(); ++it) {
        calcBeforeAfter(it->data.identified);
        calcCollisions(it->data.identified, it->data);
        calcOpticalFlows(it->data);
        processKalmans(it->data.img.cols, it->data.img.rows, it->data);

        if (it->data.frame_idx % it->data.detect_interval == 0) {
            it->data.hog.detectMultiScale(it->data.gray, it->data.foundLocations, it->data.foundWeights,
                                          hogParams.hitThreshold,
                                          hogParams.winStride,
                                          hogParams.padding,
                                          hogParams.scale, hogParams.finalThreshold, hogParams.useMeanShift);
//            it->data.angleHog.detectMultiScale(it->data.gray, it->data.foundAngleLocations, it->data.foundAngleWeights,
//                                               angleHogParams.hitThreshold,
//                                               angleHogParams.winStride,
//                                               angleHogParams.padding,
//                                               angleHogParams.scale, angleHogParams.finalThreshold,
//                                               angleHogParams.useMeanShiftGrouping);
//            if (!it->data.foundAngleLocations.empty()) {
//                for (vector<Rect>::iterator i = it->data.foundAngleLocations.begin();
//                     i != it->data.foundAngleLocations.end(); ++i) {
//                    if (i != it->data.foundAngleLocations.end() && i != it->data.foundAngleLocations.begin() &&
//                        isEmpty(it->data.maskedGray, *i, 25)) {
//                        it->data.foundAngleLocations.erase(i);
//                    }
//                }
//            }

            for (int i = 0; i < it->data.foundLocations.size(); i++) {
                Rect trimmed = trimRect(it->data.foundLocations[i]);
                Ptr<Human> human = new Human();
                vector<Ptr<Human>> humanProposals;
                Ptr<Human> best;
                map<int, pair<int, int>> src;
                for (vector<View>::iterator iter = views.begin(); iter != views.end(); ++iter) {
                    // TODO if many views it will slooooooooooooooooooow down
                    Ptr<Human> tempHuman = new Human();
                    Ptr<Human> best = getBestMatch(human, trimmed, iter->data.identified, it->data);
                    if (!best.empty()) {
                        humanProposals.push_back(best);
                        map<int, pair<int, int>>::iterator map_it = src.find(best->data.id);
                        if (map_it != src.end()) {
                            map_it->second.second++;
                        } else {
                            src.insert(pair<int, pair<int, int>>(best->data.id, pair<int, int>(iter->id, 0)));
                        }
                    }
                }
                if (src.size() > 0) {
                    int max = 0;
                    int index = 0;
                    for (map<int, pair<int, int>>::iterator map_it = src.begin(); map_it != src.end(); map_it++) {
                        if (max < map_it->second.second) {
                            max = map_it->second.second;
                            index = map_it->second.first;
                        }
                    }
                    best = humanProposals[index];
                    if (it->id != index) {
                        vector<Ptr<Human>>::iterator humanInCurrentView;
                        for(humanInCurrentView = it->data.identified.begin(); humanInCurrentView != it->data.identified.end(); humanInCurrentView++) {
                            if(humanInCurrentView->get()->data.id == best->data.id) {
                                break;
                            }
                        }
                        if (humanInCurrentView != it->data.identified.end()) {
                            updateIdentified(trimmed, human, *humanInCurrentView, it->data);
                        } else {
                            copyIdentified(trimmed, best, it->data);
                        }
                    } else {
                        updateIdentified(trimmed, human, best, it->data);
                    }
                } else {
                    newIdentified(trimmed, human, it->data);
                }
            }
        }
    }
}

void ReidentificationAlg::calcBeforeAfter(vector<Ptr<Human>> &identified) {
    for (Ptr<Human> h : identified) {
        h->calcBeforeAfter(h->data.point);
    }
}

Ptr<Human> ReidentificationAlg::getBestMatch(Ptr<Human> &human, Rect &rect, vector<Ptr<Human>> &identified,
                                             ReidentificationData &data) {
    Ptr<Human> bestHuman;
    human->data.histDescriptor.extractFeatures(data.img, rect, data.fgMaskMOG2);
    double best_comparison = 0;
    for (vector<Ptr<Human>>::iterator it = identified.begin(); it != identified.end(); it++) {
        double comparison = human->data.histDescriptor.compare(((*it)->data.histDescriptor));
        if (comparison > COMPARISON_THRESHOLD && best_comparison < comparison) {
            best_comparison = comparison;
            bestHuman = *it;
        }
    }

    if (bestHuman.empty()) {
        return Ptr<Human>();
    }
    else {
        return bestHuman;
    }
}

bool ReidentificationAlg::isGlitch(Rect &rect1, Rect &rect2) {
    double dist = sqrt(
            (rect1.x - rect2.x) ^ 2 + (rect1.y - rect2.y) ^ 2);
    return dist > (rect1.width + rect2.width)/2 ;
}

bool ReidentificationAlg::isGlitch(Ptr<Human> &human, Ptr<Human> &bestMatch) {
    double dist = sqrt(
            (human->data.point.x - bestMatch->data.point.x) ^ 2 + (human->data.point.y - bestMatch->data.point.y) ^ 2);
    return (bestMatch->data.move.x == 0 && bestMatch->data.move.y == 0) ?
           false :
           dist > 30.0 * sqrt((float) (bestMatch->data.move.x ^ 2 + bestMatch->data.move.y ^ 2));
}

void ReidentificationAlg::newIdentified(Rect &rect, Ptr<Human> &human, ReidentificationData &data) {
    human->data.id = data.getNextId();
    human->data.point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    human->data.color = randColor();
    human->data.boudingBox = rect;
    human->data.opticalFlow.getOpticalFlowPoints(rect, data.maskedGray);
    data.identified.push_back(human);
}

void ReidentificationAlg::copyIdentified(Rect &rect, Ptr<Human> &human, ReidentificationData &data) {
    Ptr<Human> h = new Human();
    h->data.id = human->data.id;
    h->data.color = human->data.color;
    h->data.point = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    h->data.boudingBox = rect;
    h->data.opticalFlow.getOpticalFlowPoints(rect, data.maskedGray);
    h->data.histDescriptor = human->data.histDescriptor;
    data.identified.push_back(h);
}


void ReidentificationAlg::updateIdentified(Rect &trimmed, Ptr<Human> &human,
                                           Ptr<Human> &best, ReidentificationData &data) {
    if(!isGlitch(trimmed, best->data.boudingBox)) {
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
}

void ReidentificationAlg::drawing(ReidentificationData &data, String &winname) {
    draw_detections(data.drawingImage, data.foundLocations);
    draw_identified(data.drawingImage, data.identified);
    draw_kalmans(data.drawingImage, data.identified);
    draw_points(data.drawingImage, data.identified);


    imshow(winname, data.drawingImage);
}

void ReidentificationAlg::calcCollisions(vector<Ptr<Human>> &identified, ReidentificationData &data) {
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

void ReidentificationAlg::draw_kalmans(Mat &img, vector<Ptr<Human>> &identified) {
    for (Ptr<Human> h: identified) {
        if (h->data.kalman.data.found) {
            h->data.kalman.drawPredicted(img);
        }
    }
}

void ReidentificationAlg::draw_points(Mat &img, vector<Ptr<Human>> &identified) {
    for (Ptr<Human> h : identified) {
        h->data.opticalFlow.drawPoints(img, h->data.color);
    }
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

Rect ReidentificationAlg::trimRect(Rect &rect) {
    int pad_w = (int) (0.15 * rect.width);
    int pad_h = (int) (0.05 * rect.height);
    Point pt1(rect.x + pad_w, rect.y + pad_h);
    Point pt2(rect.x + rect.width - pad_w, rect.y + rect.height - pad_h);
    return Rect(pt1, pt2);
}

void ReidentificationAlg::draw_detections(Mat &frame, vector<Rect> &rects) {
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

void ReidentificationAlg::draw_identified(Mat &img, vector<Ptr<Human>> &identified) {
    for (Ptr<Human> human : identified) {
        if (human->data.kalman.data.found) {
            string str = "ID:";
            str.append(to_string(human->data.id));

            putText(img, str, human->data.predicted, CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
            circle(img, human->data.predicted, 4, human->data.color);
        }
    }
}

void ReidentificationAlg::finish(ReidentificationData &data, const String &winname) {
    clear(data);

    destroyWindow(winname);

    delete data.cap;
}

void ReidentificationAlg::clear(ReidentificationData &data) {
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
        cout << rect.width << " " << rect.height << " " << rect.x << " " << rect.y << endl;
        Point p(rand() % rect.width, rand() % rect.height);
        Vec3f c = img.at<Vec3f>(p);
        if (c(0) == 0 && c(1) == 0 && c(2) == 0) {
            count++;
        }
    }
    return (double) count / (density * density) > EMPTY_LIMIT;
}

int ReidentificationAlg::getIndexIfHumanExists(vector<Ptr<Human>> identified, int id) {
    for (int i = 0; i < identified.size(); i++) {
        if (identified[i]->data.id == id) {
            return i;
        }
    }
    return -1;
}













