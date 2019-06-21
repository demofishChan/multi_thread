#include "thread_use_case.h"
#include <iostream>

using namespace cv;

void useCaseThread::initThread() {
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();

    // mutex
    pthread_mutex_init(&srcMutex, NULL);
    pthread_mutex_init(&depthMutex, NULL);
    pthread_mutex_init(&depthCopyMutex, NULL);
    pthread_cond_init(&srcCond, NULL);
    pthread_cond_init(&depthCond, NULL);
}

void useCaseThread::endThread() {
    // mutex
    pthread_mutex_destroy(&srcMutex);
    pthread_mutex_destroy(&depthMutex);
    pthread_mutex_destroy(&depthCopyMutex);
    pthread_cond_destroy(&srcCond);
    pthread_cond_destroy(&depthCond);

    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    // pthread_exit(NULL);
}

void useCaseThread::init(bokeh_handle_t* handle) {
    mPtrBokehHandle = handle;
    start();
}

void useCaseThread::run() {
    mFlagRunning = true;
    initThread();
    while (mFlagRunning) {

        lockSrc();
        //线程在此挂起，等待src数据，主线程会将src数据拷贝到目标地址后，再发射信号
        waitSrc();
        mProcessingDepth = true;
        //退出信号
        if (! mFlagRunning) {
            mProcessingDepth = false;
            unlockSrc();
            continue;
        }
        unlockSrc();
        
        lockDepth();
        mDepthReady = false;
        
        //call depth function
        processDepth();

        // set depth ready flag
        mDepthReady = true; 
        unlockDepth();
        mProcessingDepth = false;
        // signalDepth();
    }
    mProcessingDepth = false;
    endThread(); 
}

int useCaseThread::processDepth(){
    //input buffer in mPtrBokehHandle->imageCurSrc;
    
    //processing depth

    //generate depth in mPtrBokehHandle->imagePreDepth;
}

// mutex & cond
void useCaseThread::lockSrc() {
    pthread_mutex_lock(&srcMutex);
}
void useCaseThread::unlockSrc() {
    pthread_mutex_unlock(&srcMutex);
}
void useCaseThread::lockDepth() {
    pthread_mutex_lock(&depthMutex);
}
void useCaseThread::unlockDepth() {
    pthread_mutex_unlock(&depthMutex);
}

void useCaseThread::lockDepthCopy() {
    pthread_mutex_lock(&depthCopyMutex);
}
void useCaseThread::unlockDepthCopy() {
    pthread_mutex_unlock(&depthCopyMutex);
}

void useCaseThread::signalSrc() {
    pthread_cond_signal(&srcCond);
}
void useCaseThread::waitSrc() {
    pthread_cond_wait(&srcCond, &srcMutex);
}
void useCaseThread::signalDepth() {
    pthread_cond_signal(&depthCond);
}
void useCaseThread::waitDepth() {
    pthread_cond_wait(&depthCond, &depthMutex);
}

void useCaseThread::updateSrc(image_t* p_image_src) {
    //memcpy(&mImageSrc, p_image_src, sizeof(cv_image_t));
}

void useCaseThread::getDepth(image_t*  p_image_depth) {
    // memcpy(p_image_depth, &mImageDepth, sizeof(cv_image_t));
}

 bool useCaseThread::isProcessingDepth() {
     return mProcessingDepth;
 }

 bool useCaseThread::isDepthReady() {
     return mDepthReady;
 }
 bool ST_SegmentThread::isThreadRunning() {
     return mFlagRunning;
 }


void ST_SegmentThread::quit() {
    mFlagRunning = false;
    signalSrc();
}






