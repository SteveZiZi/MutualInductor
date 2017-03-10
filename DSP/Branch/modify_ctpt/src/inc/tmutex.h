#ifndef M_LIB_TMUTEX_H
#define M_LIB_TMUTEX_H

#include <pthread.h>

struct ThreadMutex
{
    ThreadMutex()
    {
        pthread_mutex_init(&mtx,NULL);
    }

    ~ThreadMutex()
    {
        pthread_mutex_destroy( &mtx );
    }

    inline void lock()
    {
        pthread_mutex_lock( &mtx );
    }

    inline void unlock()
    {
        pthread_mutex_unlock( &mtx );
    }

    pthread_mutex_t mtx;
};  

//空互斥量，即调用lock时什么事都不做。                                        
struct NullMutex                                              
{                                                             
    inline void lock()                                    
    {                                                     
    }                                                     
    inline void unlock()                                  
    {                                                     
    }                                                     
};  

#endif
