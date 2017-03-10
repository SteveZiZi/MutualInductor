#ifndef AUTO_LOCK_H
#define AUTO_LOCK_H

#include <pthread.h>

class auto_lock
{
public:
    auto_lock(pthread_mutex_t* mutex) : mutex(mutex){
        pthread_mutex_lock(this->mutex);
    }
    ~auto_lock() {
        pthread_mutex_unlock(this->mutex);
    }
private:
    pthread_mutex_t *mutex;
};

#endif // AUTO_LOCK_H
