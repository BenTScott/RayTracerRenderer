#ifndef INCLUDE_TASKMONITOR_H_
#define INCLUDE_TASKMONITOR_H_

#include <iostream>

class TaskMonitor
{
  public:
    TaskMonitor(double update_increment = 0.05) : update_increment(update_increment){};
    ~TaskMonitor(){};

    TaskMonitor(TaskMonitor &obj)
    {
        total_job_count = obj.total_job_count;
        update_increment = obj.update_increment;
    }

    void Initialise(unsigned job_count)
    {
        total_job_count = job_count;
        current_job_number = 0;
        current_increment = update_increment;
    };

    void Increment()
    {
        current_job_number++;
        if ((double)current_job_number / (double)total_job_count >= current_increment)
        {
            std::cout << current_increment * 100 << "% done\n";
            current_increment += update_increment;
        }
    };

  private:
    double update_increment;
    unsigned total_job_count;
    unsigned current_job_number;
    double current_increment;
};

#endif
