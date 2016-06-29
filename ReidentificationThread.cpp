//
// Created by hya on 27.06.16.
//

#include "ReidentificationThread.h"

void ReidentificationThread::start() {
    th = new thread(&ReidentificationThread::threadFunction, this);
    th->join();
}

void ReidentificationThread::threadFunction() {
    r->setFileName(fileName);
    r->start();
}

void ReidentificationThread::stop() {
    th->~thread();
}







