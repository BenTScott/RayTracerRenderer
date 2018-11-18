#ifndef INCLUDE_MULTITHREADEDSCENE_H_
#define INCLUDE_MULTITHREADEDSCENE_H_

#include <thread>
#include <queue>
#include <mutex>
#include "adaptivesampledscene.h"
#include "threadsafeimage.h"
#include "taskqueue.h"

class MultithreadedScene : public AdaptiveSampledScene
{
public:
  virtual ~MultithreadedScene(){};

  MultithreadedScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned max_sample_rate, SamplingMethod method, unsigned thread_count) : AdaptiveSampledScene(cam, background_colour, max_sample_rate, method), thread_count(thread_count){};

protected:
  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height) override;
  virtual RGBImage *GetSampleRates(unsigned resolution_width, unsigned resolution_height) override;
  RGBImage *FirstPass(unsigned resolution_width, unsigned resolution_height);
  unsigned thread_count;

  void SampledPixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, SamplingMethod method);
  void RowThreadTask(TaskQueue<unsigned> &queue, ThreadSafeImage *image, unsigned resolution_width);
};

#endif