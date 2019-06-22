#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include<unistd.h>

typedef struct 
{
    int img_width;
    int img_height;
    char* img_ptr;
}image_t;


typedef struct 
{
    void* p_depth_thread;
    image_t imageCurSrc; //depth 线程入图
    image_t imagePreSrc; 
    image_t imageCurDepth; //depth 线程结果，用于blur线程的输入
    image_t imagePreDepth; //depth 线程结果，用于depth线程输出
    image_t imageBlurRes;
}bokeh_handle_t;

#define DEBUG(...) printf(__VA_ARGS__);printf("\n");

#endif