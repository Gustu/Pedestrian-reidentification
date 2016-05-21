//
// Created by hya on 16.05.16.
//

#include "FilterBank.h"


FilterBank &FilterBank::getInstance() {
    static FilterBank filterBank;
    return filterBank;
}

