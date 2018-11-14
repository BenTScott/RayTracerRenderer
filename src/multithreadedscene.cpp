#include "multithreadedscene.h"
#include <thread>
#include <mutex>

RGBImage *MultithreadedScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    std::cout << "Performing first pass\n";

    RGBImage *first_pass = GetSampleRates(resolution_width, resolution_height);
    ThreadSafeImage *image = new ThreadSafeImage(*first_pass);
    delete first_pass;

    std::cout << "Starting render\n";

    std::thread threads[thread_count];

    TaskQueue<PixelTask> queue(tasks);

    queue.InitailiseMonitor(*monitor);

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i] = std::thread(&MultithreadedScene::ThreadTask, this, std::ref(queue), image, method);
    }

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    return image;
}

void MultithreadedScene::ThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, SamplingMethod method)
{
    PixelTask task;

    while (queue.TryDequeue(task))
    {
        std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(task.pixel_x, task.pixel_y, task.sample_rate)
                                                               : cam.GetJitterRaySamples(task.pixel_x, task.pixel_y, task.sample_rate);

        image->SetPixel(task.pixel_x, task.pixel_y, GetRayAverage(rays));
    }
}