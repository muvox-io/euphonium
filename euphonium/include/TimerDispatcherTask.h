#ifndef TIMERDISPATCHERTASK_H_
#define TIMERDISPATCHERTASK_H_
#include "EventBus.h"
#include <cstdint>
#include <map>

/**
 * The timer dispatcher is responsible for scheduling and dispatching
 * timers set from berry.
 **/
class TimerDispatcherTask : public bell::Task {
  public:
    TimerDispatcherTask();
    ~TimerDispatcherTask();
    /**
     * Holds the scheduled timers. It is a sorted map, since it is used as a
     * priority queue.
     * key: scheduled time in microseconds, value: timer id
     **/
    std::map<int64_t, int> scheduledTimers;
  protected:
    void runTask();
};

#endif // TIMERDISPATCHERTASK_H_
