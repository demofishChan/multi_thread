#ifndef _THREAD_USE_CASE_MASK_H_
#define _THREAD_USE_CASE_MASK_H_
#include "common.h"
#include "df_thread_common.h"

class useCaseThreadMask: public threadCommon {
private:
    bokeh_handle_t* mPtrBokehHandle = NULL;

    //整体线程运行状态flag
    bool mFlagRunning = false;
    pthread_mutex_t srcMutex    = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t maskMutex   = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t maskCopyMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  srcCond     = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  maskCond    = PTHREAD_COND_INITIALIZER;
   
    //状态同步
    volatile bool mMaskReady       = false; //同步mask状态是否已更新
    volatile bool mProcessingMask  = false; //控制子线程是否退出

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
    void lockMask();
    void unlockMask();
    void lockMaskCopy();
    void unlockMaskCopy();

    void signalSrc();
    void waitSrc();
    void signalMask();
    void waitMask();

    // 实现
    void updateSrc(image_t* p_image_src) ;
    void getMask(image_t*  p_image_mask);
    bool isProcessingMask();
    bool isMaskReady();
    bool isThreadRunning();

    int processMask();

    void quit(); //退出线程
};


#endif



