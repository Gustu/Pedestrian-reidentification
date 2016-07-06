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
    ReidentificationData data;
    char *fileName;
    void threadFunction();
    int rc;
    thread *th;
public:
    ReidentificationThread(char *fileName):fileName(fileName) {
        r = new ReidentificationAlg(data);
    }
    ~ReidentificationThread() {
        delete r;
    }
    void start();
    void stop();
};


#endif //REIDENTIFICATION_REIDENTIFICATIONTHREAD_H
