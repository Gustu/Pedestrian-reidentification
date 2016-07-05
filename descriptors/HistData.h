//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_HISTDATA_H
#define REIDENTIFICATION_HISTDATA_H

#include <list>
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

class HistData {
public:
    MatND full;
    vector<MatND> partials;
    int counter;
};


#endif //REIDENTIFICATION_HISTDATA_H
