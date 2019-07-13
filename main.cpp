#include <string.h>
#include "thread_use_case.h"
#include<unistd.h>

#define IMG_WIDTH 1920
#define IMG_HEIGHT 1080
#define DEPTH_WIDTH 352
#define DEPTH_HEIGHT 352

int initImage_Bytes(image_t &input, const int width, const int height)
{
    input.img_width = width;
    input.img_height= height;
    input.img_ptr   = new char[input.img_width * input.img_height * sizeof(char)];
    memset(input.img_ptr, 255, input.img_width * input.img_height * sizeof(char));
}

int deInitImage(image_t &input)
{
    if (!input.img_ptr)
    {
        delete [] input.img_ptr;
    }
    input.img_width = 0;
    input.img_height= 0;
}

int init(bokeh_handle_t ** p_bokeh_handle)
{
    *p_bokeh_handle = nullptr;
    *p_bokeh_handle = new bokeh_handle_t;
    // segment thread
    useCaseThread *depth_thread = new useCaseThread();
    
    //创建共享内存，内存统一管理
    initImage_Bytes((*p_bokeh_handle)->imagePreSrc, IMG_WIDTH, IMG_HEIGHT);
    initImage_Bytes((*p_bokeh_handle)->imageCurSrc, IMG_WIDTH, IMG_HEIGHT);
    initImage_Bytes((*p_bokeh_handle)->imagePreDepth, DEPTH_WIDTH, DEPTH_HEIGHT);
    initImage_Bytes((*p_bokeh_handle)->imageCurDepth, DEPTH_WIDTH, DEPTH_HEIGHT);

    depth_thread->init(*p_bokeh_handle);//创建线程
    (*p_bokeh_handle)->p_depth_thread = depth_thread;
}

int deInit(bokeh_handle_t * p_bokeh_handle)
{
    if(!p_bokeh_handle)
    {
        deInitImage(p_bokeh_handle->imagePreSrc);
        deInitImage(p_bokeh_handle->imageCurSrc);
        deInitImage(p_bokeh_handle->imagePreDepth);
        deInitImage(p_bokeh_handle->imagePreDepth);

        p_bokeh_handle->p_depth_thread = nullptr;
        delete p_bokeh_handle;
    }
}

int blur(bokeh_handle_t p_bokeh_handle, char * p_cur_frame)
{
    //depth:p_bokeh_handle->imageCurDepth
    memset(p_bokeh_handle.imageCurDepth.img_ptr, 0, 
        p_bokeh_handle.imageCurDepth.img_width * p_bokeh_handle.imageCurDepth.img_height);
    sleep(1); //模拟处理时间
    DEBUG("Bluring...\n");
    return 0;
}


int main()
{
    bokeh_handle_t * p_bokeh_handle = nullptr;
    DEBUG("main start\n");
    init(&p_bokeh_handle);
    DEBUG("init finish\n");
    DEBUG("p_bokeh_handle:%x\n", p_bokeh_handle);

    int frameSize = IMG_WIDTH * IMG_HEIGHT;
    char p_cur_frame[IMG_WIDTH * IMG_HEIGHT];

    sleep(1); //等待，必须要让子线程先进入wait状态

    //模拟帧数运行
    for (int i = 0; i < 100; i++)
    {
        DEBUG("%d\n", i);
        useCaseThread *p_depth_thread =
            (useCaseThread *)p_bokeh_handle->p_depth_thread;
        
        if(0 == i)
        {
            //更新depth线程的入参图
            p_depth_thread->lockSrc();
            //DEBUG("copy source image...\n");
            memcpy(p_bokeh_handle->imageCurSrc.img_ptr, 
                p_cur_frame, 
                p_bokeh_handle->imageCurSrc.img_width * p_bokeh_handle->imageCurSrc.img_height);
            p_depth_thread->unlockSrc();

            //启动子线程进行depth处理
            p_depth_thread->signalSrc();
        }

        //DEBUG("check depth...\n");
        //若子线程depth结果可用，则更新depth
        if (p_depth_thread->isDepthReady())
        {
            //获取depth信息
            p_depth_thread->lockDepth();
            //DEBUG("copy depth...\n");
            memcpy(p_bokeh_handle->imageCurDepth.img_ptr, 
                p_bokeh_handle->imagePreDepth.img_ptr,
                p_bokeh_handle->imageCurDepth.img_width * p_bokeh_handle->imageCurDepth.img_height);
            p_depth_thread->unlockDepth();

            //更新depth线程的入参图
            p_depth_thread->lockSrc();
            //DEBUG("copy source image...\n");
            memcpy(p_bokeh_handle->imageCurSrc.img_ptr, 
                p_cur_frame, 
                p_bokeh_handle->imageCurSrc.img_width * p_bokeh_handle->imageCurSrc.img_height);
            p_depth_thread->unlockSrc();
            
            //给depth线程发信号
            //DEBUG("signal...\n");
            p_depth_thread->signalSrc();
        }

        //blur
        //DEBUG("begin to blur...\n");
        blur(*p_bokeh_handle, p_cur_frame);
    }

    deInit(p_bokeh_handle);
    return 0;
}






