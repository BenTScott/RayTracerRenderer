#ifndef INCLUDE_TASKQUEUE_H_
#define INCLUDE_TASKQUEUE_H_

#include "taskmonitor.h"

// A thread safe job queue for feeding parrallel threads.

template <typename T>
class TaskQueue
{
  public:
    TaskQueue(std::vector<T> &tasks) : queue(std::deque<T>(tasks.begin(), tasks.end())), monitor(nullptr){};;
    TaskQueue() : monitor(nullptr){};;

    ~TaskQueue(){};

    bool TryDequeue(T &task)
    {
        std::lock_guard<std::mutex> lock(m);
        if (queue.empty())
        {
            return false;
        }

        task = std::move(queue.front());
        queue.pop();

        if (monitor)
        {
            monitor->Increment();
        }

        return true;
    };

    void Push(T task)
    {
        std::lock_guard<std::mutex> lock(m);
        queue.push(task);
    }

    void InitailiseMonitor(TaskMonitor monitor)
    {
        this->monitor = new TaskMonitor(monitor);
        this->monitor->Initialise(this->Size());
    }

    unsigned Size()
    {
        return queue.size();
    };

  private:
    std::queue<T> queue;
    TaskMonitor *monitor;
    std::mutex m;

};

#endif