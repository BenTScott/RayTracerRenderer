#include "multithreadedscene.h"
#include <thread>
#include <mutex>

RGBImage *MultithreadedScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    std::cout << "Performing first pass\n";

    RGBImage *image = GetSampleRates(resolution_width, resolution_height);

    std::cout << "Starting render\n";

    cam.InitialiseResolution(resolution_width, resolution_height);

    unsigned total_pixels = resolution_height * resolution_width;
    monitor->Initialise(total_pixels);

    std::thread threads[thread_count];

    TaskQueue queue(sample_rates);

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i] = std::thread(ThreadTask, queue, image, method);
    }

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    return image;
}

void MultithreadedScene::ThreadTask(TaskQueue &queue, RGBImage *image, SamplingMethod method)
{
    TaskQueue::Task task;

    if (!queue.TryDequeue(task))
        return;

    std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(task.pixel_x, task.pixel_y, task.sample_rate)
                                                           : cam.GetJitterRaySamples(task.pixel_x, task.pixel_y, task.sample_rate);

    image->SetPixel(task.pixel_x, task.pixel_y, GetRayAverage(rays));
}