//
// Created by hya on 27.06.16.
//

#ifndef REIDENTIFICATION_REIDENTIFICATIONTHREAD_H
#define REIDENTIFICATION_REIDENTIFICATIONTHREAD_H

#include <thread>
#include "ReidentificationAlg.h"

class ReidentificationThread {
private:
    ReidentificationAlg *r;
    char *fileName;
    void threadFunction();
    int rc;
    thread *th;
public:
    ReidentificationThread(char *fileName):fileName(fileName) {
        r = new ReidentificationAlg();
    }
    ~ReidentificationThread() {
        delete r;
    }
    void start();
    void stop();
};


#endif //REIDENTIFICATION_REIDENTIFICATIONTHREAD_H
