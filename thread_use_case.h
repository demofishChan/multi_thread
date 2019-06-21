#ifndef _THREAD_USE_CASE_H_
#define _THREAD_USE_CASE_H_
#include "common.h"
#include "df_thread_common.h"

class useCaseThread: public threadCommon {
private:
    bokeh_handle_t* mPtrBokehHandle = NULL;

    //整体线程运行状态flag
    bool mFlagRunning = false;
    pthread_mutex_t srcMutex    = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t depthMutex  = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t depthCopyMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  srcCond     = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  depthCond   = PTHREAD_COND_INITIALIZER;
   
    //状态同步
    volatile bool mDepthReady       = false; //同步depth状态是否已更新
    volatile bool mProcessingDepth  = false; //控制子线程是否退出

    // function
    void initThread();
    void endThread();
    
public:
    void init(bokeh_handle_t* handle);

    // parent
    void run();

    // mutex
    void lockSrc();
    void unlockSrc();
    void lockDepth();
    void unlockDepth();
    void lockDepthCopy();
    void unlockDepthCopy();

    void signalSrc();
    void waitSrc();
    void signalDepth();
    void waitDepth();

    // 实现
    void updateSrc();
    void getDepth();
    bool isProcessingDepth();
    bool isDepthReady();
    bool isThreadRunning();

    int processDepth();

    void quit(); //退出线程
};


#endif



