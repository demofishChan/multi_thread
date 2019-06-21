#include "df_thread.h"
#include <unistd.h>
 
void* threadCommon::run1()
{
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();
    run();
    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    pthread_exit(NULL);
}
 
threadCommon::Thread()
{
    tid = 0;
    threadStatus = THREAD_STATUS_NEW;
}
 
bool threadCommon::start()
{
	int iRet = 0;
    pthread_create(&tid, NULL, thread_proxy_func, this) == 0;
}
 
pthread_t threadCommon::getThreadID()
{
    return tid;
}
 
int threadCommon::getState()
{
    return threadStatus;
}
 
void threadCommon::join()
{
    if (tid != 0)
    {
        pthread_join(tid, NULL);
    }
}
void * threadCommon::thread_proxy_func(void * args)
{
 	Thread * pThread = static_cast<Thread *>(args); 
	pThread->run(); 
 	return NULL; 
}
 
void threadCommon::join(unsigned long millisTime)
{
    if (tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    } else {
        unsigned long k = 0;
        while (threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            usleep(100);
            k++;
        }
    }
}
