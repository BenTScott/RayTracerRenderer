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

    cam.InitialiseResolution(resolution_width, resolution_height);

    unsigned total_pixels = resolution_height * resolution_width;
    monitor->Initialise(total_pixels);

    std::thread threads[thread_count];

    TaskQueue queue(sample_rates);

    unsigned total_tasks = queue.Size();

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i] = std::thread(&MultithreadedScene::ThreadTask, this, std::ref(queue), image, method);
    }

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    std::cout << queue.Size();

    return image;
}

void MultithreadedScene::ThreadTask(TaskQueue &queue, ThreadSafeImage *image, SamplingMethod method)
{
    TaskQueue::Task task;

    while (queue.TryDequeue(task))
    {
        std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(task.pixel_x, task.pixel_y, task.sample_rate)
                                                            : cam.GetJitterRaySamples(task.pixel_x, task.pixel_y, task.sample_rate);

        image->SetPixel(task.pixel_x, task.pixel_y, GetRayAverage(rays));
    }
}