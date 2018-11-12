#ifndef INCLUDE_MULTITHREADEDSCENE_H_
#define INCLUDE_MULTITHREADEDSCENE_H_

#include <thread>
#include <queue>
#include <mutex>
#include "adaptivesampledscene.h"
#include "threadsafeimage.h"

class MultithreadedScene : public AdaptiveSampledScene
{
public:
  virtual ~MultithreadedScene(){};

  MultithreadedScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned max_sample_rate, SamplingMethod method, unsigned thread_count) : AdaptiveSampledScene(cam, background_colour, max_sample_rate, method), thread_count(thread_count){};

protected:
  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height) override;
  unsigned thread_count;

  class TaskQueue
  {
  public:
    struct Task
    {
      unsigned pixel_x;
      unsigned pixel_y;
      unsigned sample_rate;
    };

    TaskQueue(std::vector<std::vector<unsigned>> &sample_rates)
    {
      for (size_t i = 0; i < sample_rates.size(); ++i)
      {
        for (size_t j = 0; j < sample_rates[0].size(); ++j)
        {
          if (sample_rates[i][j] > 1)
          {
            Task task;
            task.pixel_x = i;
            task.pixel_y = j;
            task.sample_rate = sample_rates[i][j];
            task_queue.push(task);
          }
        }
      }
    }

    ~TaskQueue(){};

    bool TryDequeue(Task &task)
    {
      std::lock_guard<std::mutex> lock(m);
      if (task_queue.empty())
      {
        return false;
      }

      task = std::move(task_queue.front());
      task_queue.pop();
      return true;
    };

    unsigned Size()
    {
      return task_queue.size();
    };

  private:
    std::mutex m;
    std::queue<Task> task_queue;
    TaskQueue();
  };

  void ThreadTask(TaskQueue &queue, ThreadSafeImage *image, SamplingMethod method);
};

#endif