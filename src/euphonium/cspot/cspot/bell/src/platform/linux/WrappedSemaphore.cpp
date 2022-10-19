#include "platform/WrappedSemaphore.h"

WrappedSemaphore::WrappedSemaphore(int count)
{
    sem_init(&this->semaphoreHandle, 0, 0); // eek pointer
}

WrappedSemaphore::~WrappedSemaphore()
{
    sem_destroy(&this->semaphoreHandle);
}

int WrappedSemaphore::wait()
{
    sem_wait(&this->semaphoreHandle);
    return 0;
}

int WrappedSemaphore::twait(long milliseconds)
{
    // wait on semaphore with timeout
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += (milliseconds % 1000) * 1000000;
    return sem_timedwait(&this->semaphoreHandle, &ts);
}

void WrappedSemaphore::give()
{
    sem_post(&this->semaphoreHandle);
}
