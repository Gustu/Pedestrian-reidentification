//
// Created by hya on 16.05.16.
//

#ifndef REIDENTIFICATION_FILTERBANK_H
#define REIDENTIFICATION_FILTERBANK_H

#include <opencv2/core/mat.hpp>
#include <cv.hpp>

using namespace std;
using namespace cv;

#define KERNEL_SIZE 15
#define RANGE 4

class FilterBank {
public:
    static FilterBank &getInstance();

    vector<Mat> kernels;

    FilterBank() {
        double thetas[RANGE] = {0, M_PI / 6, M_PI / 3, M_PI / 2};
        double lambdas[RANGE] = {2, 12, 30, 56};
        double sigmas[RANGE] = {4, 8, 12, 16};

        for (int i = 0; i < RANGE; i++) {
            for (int j = 0; j < RANGE; j++) {
                for (int k = 0; k < RANGE; k++) {
                    Mat kernel = getGaborKernel(Size(KERNEL_SIZE, KERNEL_SIZE), sigmas[i], thetas[i], lambdas[i], lambdas[i]/sigmas[i]);
                    kernels.push_back(kernel);
                }
            }
        }
    }
};


#endif //REIDENTIFICATION_FILTERBANK_H
