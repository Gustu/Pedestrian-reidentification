//
// Created by hya on 29.06.16.
//

#ifndef REIDENTIFICATION_HOGTRAINER_H
#define REIDENTIFICATION_HOGTRAINER_H

#define TRAINED_FILE "hog.yml"

#include <opencv2/opencv.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <time.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;
using namespace cv::ml;
using namespace std;

class HOGTrainer {
public:
    string posDir;
    string pos;
    string negDir;
    string neg;
    string trained;

    void train();
    HOGDescriptor getHOG();
    HOGTrainer(const Size &size): size(size), trained(TRAINED_FILE) {}
    HOGTrainer(string posDir, string pos, string negDir, string neg, Size size): posDir(posDir), pos(pos), negDir(negDir),
                                                                      neg(neg), size(size), trained(TRAINED_FILE) {}
    void testIt(const string fileName);
    Size size;
private:
    void getSVMDetector(const Ptr<SVM>& svm, vector< float > & hog_detector );
    void convertToMl(const std::vector< cv::Mat > & train_samples, cv::Mat& trainData );
    void loadImages( const string & dir, const string & listFile, vector< Mat > & img_lst );
    void sampleNeg( const vector< Mat > & full_neg_lst, vector< Mat > & neg_lst, const Size & size );
    Mat getHogdescriptorVisu(const Mat& color_origImg, vector<float>& descriptorValues, const Size & size );
    void computeHog( const vector< Mat > & img_lst, vector< Mat > & gradient_lst, const Size & size );
    void trainSvm( const vector< Mat > & gradient_lst, const vector< int > & labels );
    void drawLocations( Mat & img, const vector< Rect > & locations, const Scalar & color );
    void samplePos(vector<Mat> &fullPosLst, vector<Mat> &posLst, Size &size);
};


#endif //REIDENTIFICATION_HOGTRAINER_H
