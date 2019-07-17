#include "thread_use_case_mask.h"
#include <iostream>

void useCaseThreadMask::initThread() {
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();

    // mutex
    pthread_mutex_init(&srcMutex, NULL);
    pthread_mutex_init(&maskMutex, NULL);
    pthread_mutex_init(&maskCopyMutex, NULL);
    pthread_cond_init(&srcCond, NULL);
    pthread_cond_init(&maskCond , NULL);
}

void useCaseThreadMask::endThread() {
    // mutex
    pthread_mutex_destroy(&srcMutex);
    pthread_mutex_destroy(&maskMutex);
    pthread_mutex_destroy(&maskCopyMutex);
    pthread_cond_destroy(&srcCond);
    pthread_cond_destroy(&maskCond );

    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    // pthread_exit(NULL);
}

void useCaseThreadMask::init(bokeh_handle_t* handle) {
    mPtrBokehHandle = handle;
    start();
}

void useCaseThreadMask::run() {
    mFlagRunning = true;
    initThread();
    while (mFlagRunning) {

        lockSrc();
        //线程在此挂起，等待src数据，主线程会将src数据拷贝到目标地址后，再发射信号
        DEBUG("IN mask thread waitting...\n");
        waitSrc();
        mProcessingMask = true;
        //退出信号
        if (! mFlagRunning) {
            mProcessingMask = false;
            unlockSrc();
            continue;
        }
        unlockSrc();
        
        lockMask();
        mMaskReady = false;
        
        //call depth function
        processMask();

        // set depth ready flag
        mMaskReady = true; 
        unlockMask();
        mProcessingMask = false;
        // signalMask();
    }
    mProcessingMask = false;
    endThread(); 
}

int useCaseThreadMask::processMask(){
    //input buffer in mPtrBokehHandle->imageCurSrc;
    
    //processing depth
    DEBUG("~~~~~~~~~~~~~~~~~~ in processMask...\n");
    sleep(2);//模拟处理时间

    //generate depth in mPtrBokehHandle->imagePreDepth;
}

// mutex & cond
void useCaseThreadMask::lockSrc() {
    pthread_mutex_lock(&srcMutex);
}
void useCaseThreadMask::unlockSrc() {
    pthread_mutex_unlock(&srcMutex);
}
void useCaseThreadMask::lockMask() {
    pthread_mutex_lock(&maskMutex);
}
void useCaseThreadMask::unlockMask() {
    pthread_mutex_unlock(&maskMutex);
}

void useCaseThreadMask::lockMaskCopy() {
    pthread_mutex_lock(&maskCopyMutex);
}
void useCaseThreadMask::unlockMaskCopy() {
    pthread_mutex_unlock(&maskCopyMutex);
}

void useCaseThreadMask::signalSrc() {
    pthread_cond_signal(&srcCond);
}
void useCaseThreadMask::waitSrc() {
    pthread_cond_wait(&srcCond, &srcMutex);
}
void useCaseThreadMask::signalMask() {
    pthread_cond_signal(&maskCond );
}
void useCaseThreadMask::waitMask() {
    pthread_cond_wait(&maskCond , &maskMutex);
}

void useCaseThreadMask::updateSrc(image_t* p_image_src) {
    //memcpy(&mImageSrc, p_image_src, sizeof(cv_image_t));
}

void useCaseThreadMask::getMask(image_t*  p_image_mask) {
    // memcpy(p_image_mask, &mImageDepth, sizeof(cv_image_t));
}

 bool useCaseThreadMask::isProcessingMask() {
     return mProcessingMask;
 }

 bool useCaseThreadMask::isMaskReady() {
     return mMaskReady;
 }
 bool useCaseThreadMask::isThreadRunning() {
     return mFlagRunning;
 }


void useCaseThreadMask::quit() {
    mFlagRunning = false;
    signalSrc();
}






