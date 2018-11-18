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
        threads[i] = std::thread(&MultithreadedScene::SampledPixelThreadTask, this, std::ref(queue), image, method);
    }

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    return image;
}

RGBImage *MultithreadedScene::GetSampleRates(unsigned resolution_width, unsigned resolution_height)
{
    // Do a multi-threaded first pass
    RGBImage *image = FirstPass(resolution_width, resolution_height);

    RGBImage *copy = new RGBImage(*image);

    ApplyKernelTransformations(copy);

    BuildTaskList(copy);

    delete copy;

    return image;
};

RGBImage *MultithreadedScene::FirstPass(unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage *image = new RGBImage(resolution_width, resolution_height);

    unsigned total_pixels = resolution_height * resolution_width;
    monitor->Initialise(total_pixels);

    std::vector<unsigned> rows;
    rows.reserve(resolution_height);
    for (unsigned i = 0; i < resolution_height; ++i)
    {
        rows.push_back(resolution_height);
    }

    TaskQueue<unsigned> queue(rows);

    std::thread threads[thread_count];

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i] = std::thread(&MultithreadedScene::ThreadTask, this, std::ref(queue), image, method);
    }

    for (unsigned i = 0; i < thread_count; ++i)
    {
        threads[i].join();
    }

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            if (monitor)
            {
                monitor->Increment();
            }

            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            image->SetPixel(i, j, GetColour(ray));
        }
    }

    return image;
}

void MultithreadedScene::SampledPixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, SamplingMethod method)
{
    PixelTask task;

    while (queue.TryDequeue(task))
    {
        std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(task.pixel_x, task.pixel_y, task.sample_rate)
                                                               : cam.GetJitterRaySamples(task.pixel_x, task.pixel_y, task.sample_rate);

        image->SetPixel(task.pixel_x, task.pixel_y, GetRayAverage(rays));
    }
}

void MultithreadedScene::RowThreadTask(TaskQueue<unsigned> &queue, ThreadSafeImage *image, unsigned resolution_width)
{
    unsigned row_index;

    while (queue.TryDequeue(row_index))
    {
        for (unsigned i = 0; i < resolution_width; ++i)
        {
            Ray ray = cam.GetRay(i, row_index);
            image->SetPixel(task.pixel_x, task.pixel_y, GetColour(ray));
        }
    }
}

