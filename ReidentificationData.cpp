//
// Created by hya on 12.07.16.
//

#include "ReidentificationData.h"

int ReidentificationData::sNextId = 0;

int ReidentificationData::getNextId() {
    return sNextId++;
}

