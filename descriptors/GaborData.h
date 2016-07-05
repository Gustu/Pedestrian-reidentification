//
// Created by hya on 05.07.16.
//

#ifndef REIDENTIFICATION_GABORDATA_H
#define REIDENTIFICATION_GABORDATA_H

#include "FilterBank.h"
using namespace std;

class GaborData {
public:
    vector<double> means;
    vector<double> stdevs;
    FilterBank filterBank;
};


#endif //REIDENTIFICATION_GABORDATA_H
